#include "windows/updatedialog.h"
#include "widgets/loadingwidget.h"
#include "base/application.h"
#include "base/emptyitemproxymodel.h"
#include "base/settings.h"
#include "base/updateitemsmodel.h"
#include "base/utils.h"
#include <QBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkReply>
#include <QPushButton>
#include <QTableView>

UpdateDialog::UpdateDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Updates"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(Utils::scale(700, 240));

    auto loadingWidget = new LoadingWidget(this);
    loadingWidget->hide();

    updatesModel = new UpdateItemsModel(this);

    table = new QTableView(this);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionBehavior(QTableView::SelectRows);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setModel(new EmptyItemProxyModel(updatesModel, this));
    table->setShowGrid(false);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->verticalHeader()->hide();

    for (int row = 0; row < updatesModel->rowCount(); ++row) {
        auto btnWhatsNew = new QLabel(this);
        btnWhatsNew->setAutoFillBackground(true);
        btnWhatsNew->setOpenExternalLinks(true);
        btnWhatsNew->setContentsMargins(0, 0, 16, 0);
        table->setIndexWidget(table->model()->index(row, UpdateItemsModel::WhatsNewUrlColumn), btnWhatsNew);
        refreshWhatsNewButton(row);

        auto btnUpdate = new QPushButton(tr("Update"));
        btnUpdate->setAutoFillBackground(true);
        table->setIndexWidget(table->model()->index(row, UpdateItemsModel::HasUpdatesColumn), btnUpdate);
        refreshUpdateButton(row);
        connect(btnUpdate, &QPushButton::clicked, this, [=]() {
            updatesModel->download(row, this);
        });
    }

    connect(updatesModel, &UpdateItemsModel::dataChanged, this, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
            refreshUpdateButton(row);
            refreshWhatsNewButton(row);
        }
    });

    connect(updatesModel, &UpdateItemsModel::fetching, this, [=]() {
        loadingWidget->show();
    });

    connect(updatesModel, &UpdateItemsModel::fetched, this, [=](bool success, const QString &error) {
        loadingWidget->hide();
        if (!success && isVisible()) {
            QMessageBox::warning(this, {}, QString("%1\n%2").arg(tr("Could not check for updates:"), error));
        }
    });

    table->resizeColumnsToContents();

    auto btnRefresh = new QPushButton(this);
    btnRefresh->setText(app->translate("ActionProvider", "Check for &Updates"));
    btnRefresh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(btnRefresh, &QPushButton::clicked, updatesModel, &UpdateItemsModel::refresh);

    auto checkboxAutoUpdate = new QCheckBox(this);
    checkboxAutoUpdate->setText(app->translate("OptionsDialog", "Check for updates automatically"));
    checkboxAutoUpdate->setChecked(app->settings->getAutoUpdates());

    auto layoutConfig = new QHBoxLayout;
    layoutConfig->addWidget(checkboxAutoUpdate);
    layoutConfig->addStretch();
    layoutConfig->addWidget(btnRefresh);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &UpdateDialog::accept);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(table);
    layout->addLayout(layoutConfig);
    layout->addWidget(buttons);

    connect(this, &UpdateDialog::accepted, this, [=]() {
        app->settings->setAutoUpdates(checkboxAutoUpdate->isChecked());
    });
}

void UpdateDialog::checkUpdates()
{
    updatesModel->refresh();
}

void UpdateDialog::refreshUpdateButton(int row)
{
    const auto updateIndex = table->model()->index(row, UpdateItemsModel::HasUpdatesColumn);
    const bool hasUpdates = updateIndex.data(Qt::UserRole).toBool();
    const auto btnUpdate = qobject_cast<QPushButton *>(table->indexWidget(updateIndex));
    btnUpdate->setEnabled(hasUpdates);
    if (hasUpdates) {
        open();
    }
}

void UpdateDialog::refreshWhatsNewButton(int row)
{
    const auto whatsNewIndex = table->model()->index(row, UpdateItemsModel::WhatsNewUrlColumn);
    const auto whatsNewUrl = whatsNewIndex.data(Qt::UserRole).toString();
    const auto btnWhatsNew = qobject_cast<QLabel *>(table->indexWidget(whatsNewIndex));
    btnWhatsNew->setText(QString("<a href=\"%1\">%2</a>").arg(whatsNewUrl, tr("What's New")));
    btnWhatsNew->setEnabled(!whatsNewUrl.isEmpty());
}
