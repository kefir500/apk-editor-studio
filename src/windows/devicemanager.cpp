#include "windows/devicemanager.h"
#include "widgets/loadingwidget.h"
#include "base/utils.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QPushButton>

DeviceManager::DeviceManager(QWidget *parent) : QDialog(parent)
{
    //: This string refers to multiple devices (as in "Manager of devices").
    setWindowTitle(tr("Device Manager"));
    setWindowIcon(QIcon::fromTheme("smartphone"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(Utils::scale(560, 300));

    QLabel *caption = new QLabel(tr("Select a device:"));

    deviceList = new QListView(this);
    deviceList->setModel(&deviceModel);
    deviceList->setCurrentIndex(QModelIndex());
    auto loading = new LoadingWidget(deviceList);

    QPushButton *btnRefresh = new QPushButton(tr("Refresh"), this);
    btnRefresh->setIcon(QIcon::fromTheme("view-refresh"));

    QVBoxLayout *listLayout = new QVBoxLayout;
    listLayout->addWidget(caption);
    listLayout->addWidget(deviceList);
    listLayout->addWidget(btnRefresh);

    fieldAlias = new QLineEdit(this);
    fieldAlias->setPlaceholderText(tr("Custom name"));
    labelSerial = new QLabel(this);
    labelProduct = new QLabel(this);
    labelModel = new QLabel(this);
    labelDevice = new QLabel(this);

    QGroupBox *groupInfo = new QGroupBox(tr("Device Information"), this);

    QFormLayout *infoLayout = new QFormLayout(groupInfo);
    infoLayout->addRow(deviceModel.headerData(DeviceItemsModel::AliasColumn, Qt::Horizontal).toString(), fieldAlias);
    infoLayout->addRow(deviceModel.headerData(DeviceItemsModel::SerialColumn, Qt::Horizontal).toString(), labelSerial);
    infoLayout->addRow(deviceModel.headerData(DeviceItemsModel::ProductColumn, Qt::Horizontal).toString(), labelProduct);
    infoLayout->addRow(deviceModel.headerData(DeviceItemsModel::ModelColumn, Qt::Horizontal).toString(), labelModel);
    infoLayout->addRow(deviceModel.headerData(DeviceItemsModel::DeviceColumn, Qt::Horizontal).toString(),labelDevice);

    QHBoxLayout *devicesLayout = new QHBoxLayout;
    devicesLayout->addLayout(listLayout, 2);
    devicesLayout->addWidget(groupInfo, 3);

    dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    QPushButton *btnApply = dialogButtons->button(QDialogButtonBox::Apply);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(devicesLayout);
    layout->addWidget(dialogButtons);

    connect(deviceList->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &index) {
        const auto device = deviceModel.get(index);
        setCurrentDevice(device);
    });
    connect(fieldAlias, &QLineEdit::textChanged, this, [this](const QString &alias) {
        QModelIndex index = deviceModel.index(deviceList->currentIndex().row(), DeviceItemsModel::AliasColumn);
        deviceModel.setData(index, alias);
    });
    connect(&deviceModel, &DeviceItemsModel::fetching, this, [=]() {
        loading->show();
        setEnabled(false);
    });
    connect(&deviceModel, &DeviceItemsModel::fetched, this, [=]() {
        loading->hide();
        setEnabled(true);
    });
    connect(btnRefresh, &QPushButton::clicked, &deviceModel, &DeviceItemsModel::refresh);
    connect(btnApply, &QPushButton::clicked, &deviceModel, &DeviceItemsModel::save);
    connect(dialogButtons, &QDialogButtonBox::accepted, this, &DeviceManager::accept);
    connect(dialogButtons, &QDialogButtonBox::rejected, this, &DeviceManager::reject);
    connect(this, &DeviceManager::accepted, &deviceModel, &DeviceItemsModel::save);
    connect(&deviceModel, &DeviceItemsModel::fetched, this, [this](bool success) {
        if (!success) {
            QMessageBox::warning(this, {}, tr("Could not fetch the device list."));
        }
    });

    setCurrentDevice({});
    deviceModel.refresh();
}

Device DeviceManager::selectDevice(const QString &title, const QString &action, const QIcon &icon, QWidget *parent)
{
    DeviceManager dialog(parent);
    dialog.setWindowTitle(title.isEmpty() ? tr("Select Device") : title);
    if (!icon.isNull()) {
        dialog.setWindowIcon(icon);
    }

    auto btnSelect = dialog.dialogButtons->button(QDialogButtonBox::Ok);
    btnSelect->setEnabled(false);
    if (!action.isEmpty()) {
        btnSelect->setText(action);
    }
    if (!icon.isNull()) {
        btnSelect->setIcon(icon);
    }

    connect(&dialog, &DeviceManager::currentChanged, btnSelect, [btnSelect](const Device &device) {
        btnSelect->setEnabled(!device.isNull());
    });

    if (dialog.exec() == QDialog::Accepted) {
        return dialog.deviceModel.get(dialog.deviceList->currentIndex());
    }

    return {};
}

bool DeviceManager::setCurrentDevice(const Device &device)
{
    emit currentChanged(device);
    if (!device.isNull()) {
        fieldAlias->setEnabled(true);
        fieldAlias->setText(device.getAlias());
        labelSerial->setText(device.getSerial());
        labelProduct->setText(device.getProductString());
        labelModel->setText(device.getModelString());
        labelDevice->setText(device.getDeviceString());
        return true;
    } else {
        const QChar dash(0x2013);
        fieldAlias->setEnabled(false);
        fieldAlias->setText("");
        labelSerial->setText(dash);
        labelProduct->setText(dash);
        labelModel->setText(dash);
        labelDevice->setText(dash);
        return false;
    }
}
