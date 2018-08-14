#include "windows/settingsdialog.h"
#include "windows/devicemanager.h"
#include "windows/keymanager.h"
#include "widgets/toolbar.h"
#include "base/application.h"
#include <QLabel>
#include <QPushButton>
#include <QAbstractButton>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    setWindowIcon(app->loadIcon("settings.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(app->dpiAwareSize(800, 400));

    // General

    QFormLayout *pageGeneral = new QFormLayout;
    checkboxUpdates = new QCheckBox(tr("Check for updates automatically"), this);
    comboLanguages = new QComboBox(this);
    QList<Language> languages = app->getLanguages();
    foreach (const Language &language, languages) {
        comboLanguages->addItem(language.getFlag(), language.getTitle(), language.getCode());
    }
    spinboxRecent = new QSpinBox(this);
    spinboxRecent->setMinimum(0);
    spinboxRecent->setMaximum(50);
    pageGeneral->addRow(checkboxUpdates);
    pageGeneral->addRow(tr("Language:"), comboLanguages);
    pageGeneral->addRow(tr("Maximum recent files:"), spinboxRecent);

    // Repacking

    QFormLayout *pageRepack = new QFormLayout;
    fileboxApktool = new FileBox(QString(), QString(), false, this);
    fileboxOutput = new FileBox(QString(), QString(), true, this);
    fileboxFrameworks = new FileBox(QString(), QString(), true, this);
    checkboxSources = new QCheckBox(tr("Decompile source code (smali)"), this);
    pageRepack->addRow(tr("Apktool path:"), fileboxApktool);
    pageRepack->addRow(tr("Extraction path:"), fileboxOutput);
    pageRepack->addRow(tr("Frameworks path:"), fileboxFrameworks);
    pageRepack->addRow(checkboxSources);
    tr("Java path:"); // TODO For future usage

    // Signing

    QVBoxLayout *pageSign = new QVBoxLayout;
    groupSign = new QGroupBox(tr("Enable"), this);
    groupSign->setCheckable(true);
    fileboxApksigner = new FileBox(QString(), QString(), false, this);
    QFormLayout *layoutSign = new QFormLayout(groupSign);
    QPushButton *btnKeyManager = new QPushButton(tr("Open Key Manager"), this);
    btnKeyManager->setIcon(app->loadIcon("key.png"));
    connect(btnKeyManager, &QPushButton::clicked, [=]() {
        KeyManager keyManager(this);
        keyManager.exec();
    });
    layoutSign->addRow(tr("Tool:"), fileboxApksigner);
    layoutSign->addRow(btnKeyManager);
    pageSign->addWidget(groupSign);

    // Optimizing

    QVBoxLayout *pageZipalign = new QVBoxLayout;
    groupZipalign = new QGroupBox(tr("Enable"), this);
    groupZipalign->setCheckable(true);
    fileboxZipalign = new FileBox(QString(), QString(), false, this);
    QFormLayout *layoutZipalign = new QFormLayout(groupZipalign);
    layoutZipalign->addRow("Zipalign:", fileboxZipalign);
    pageZipalign->addWidget(groupZipalign);

    // Installing

    QFormLayout *pageInstall = new QFormLayout;
    fileboxAdb = new FileBox(QString(), QString(), false, this);
    QPushButton *btnDeviceManager = new QPushButton(tr("Open Device Manager"), this);
    btnDeviceManager->setIcon(app->loadIcon("device.png"));
    connect(btnDeviceManager, &QPushButton::clicked, [=]() {
        DeviceManager deviceManager(this);
        deviceManager.exec();
    });
    pageInstall->addRow("ADB:", fileboxAdb);
    pageInstall->addRow(btnDeviceManager);

    // Toolbar

    QHBoxLayout *pageToolbar = new QHBoxLayout;
    listToolbarUsed = new QListWidget(this);
    listToolbarUsed->setIconSize(QSize(20, 20));
    listToolbarUsed->setDragDropMode(QAbstractItemView::DragDrop);
    listToolbarUsed->setDefaultDropAction(Qt::MoveAction);
    listToolbarUnused = new PoolListWidget(this);
    connect(listToolbarUsed, &QListWidget::doubleClicked, [=](const QModelIndex &index) {
        QListWidgetItem *item = listToolbarUsed->takeItem(index.row());
        const bool reusable = item->data(PoolListWidget::ReusableRole).toBool();
        if (!reusable) {
            listToolbarUnused->addItem(item);
        }
    });
    connect(listToolbarUnused, &QListWidget::doubleClicked, [=](const QModelIndex &index) {
        QListWidgetItem *item = listToolbarUnused->item(index.row());
        const bool reusable = item->data(PoolListWidget::ReusableRole).toBool();
        listToolbarUsed->addItem(new QListWidgetItem(*item));
        if (!reusable) {
            delete item;
        }
    });

    pageToolbar->addWidget(listToolbarUsed);
    pageToolbar->addWidget(listToolbarUnused);

    // Initialize

    pageStack = new QStackedWidget(this);
    pageStack->setFrameShape(QFrame::StyledPanel);

    pageList = new QListWidget(this);
    addPage(tr("General"), pageGeneral);
    addPage(tr("Repacking"), pageRepack);
    addPage(tr("Signing APK"), pageSign);
    addPage(tr("Optimizing APK"), pageZipalign);
    addPage(tr("Installing APK"), pageInstall);
    addPage(tr("Toolbar"), pageToolbar, false);
    pageList->setCurrentRow(0);
    pageList->setMaximumWidth(pageList->sizeHintForColumn(0) + 60);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    QPushButton *btnApply = buttons->button(QDialogButtonBox::Apply);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(pageList, 0, 0);
    layout->addWidget(pageStack, 0, 1);
    layout->addWidget(buttons, 1, 0, 1, 2);

    load();

    connect(pageList, &QListWidget::currentRowChanged, pageStack, &QStackedWidget::setCurrentIndex);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    connect(btnApply, &QPushButton::clicked, this, &SettingsDialog::save);
    connect(this, &SettingsDialog::accepted, this, &SettingsDialog::save);
}

void SettingsDialog::addPage(const QString &title, QLayout *page, bool stretch)
{
    QWidget *container = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    QLabel *titleLabel = new QLabel(QString("%1").arg(title), this);
    QFrame *titleLine = new QFrame(this);
    titleLine->setFrameShape(QFrame::HLine);
    titleLine->setFrameShadow(QFrame::Sunken);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(13);
    titleLabel->setFont(titleFont);
    containerLayout->addWidget(titleLabel);
    containerLayout->addWidget(titleLine);
    containerLayout->addLayout(page);
    if (stretch) {
        containerLayout->addStretch();
    }
    pageList->addItem(title);
    pageStack->addWidget(container);
}

void SettingsDialog::load()
{
    // General

    checkboxUpdates->setChecked(app->settings->getAutoUpdates());
    spinboxRecent->setValue(app->settings->getRecentLimit());

    // Repacking

    fileboxApktool->setCurrentPath(app->settings->getApktoolPath());
    fileboxApktool->setDefaultPath(app->getToolPath("apktool.jar"));
    fileboxOutput->setCurrentPath(app->settings->getOutputDirectory());
    fileboxOutput->setDefaultPath(app->getOutputPath());
    fileboxFrameworks->setCurrentPath(app->settings->getFrameworksDirectory());
    fileboxFrameworks->setDefaultPath(app->getFrameworksPath());
    checkboxSources->setChecked(app->settings->getDecompileSources());

    // Signing

    groupSign->setChecked(app->settings->getSignApk());
    fileboxApksigner->setCurrentPath(app->settings->getApksignerPath());
    fileboxApksigner->setDefaultPath(app->getToolPath("apksigner.jar"));

    // Optimizing

    groupZipalign->setChecked(app->settings->getOptimizeApk());
    fileboxZipalign->setCurrentPath(app->settings->getZipalignPath());
    fileboxZipalign->setDefaultPath(app->getToolPath("zipalign"));

    // Installing

    fileboxAdb->setCurrentPath(app->settings->getAdbPath());
    fileboxAdb->setDefaultPath(app->getToolPath("adb"));

    // Toolbar

    listToolbarUsed->clear();
    QMap<QString, QAction *> unusedToolbarActions = app->toolbar;
    QStringList usedToolbarActions = app->settings->getToolbar();
    foreach (const QString &identifier, usedToolbarActions) {
        unusedToolbarActions.remove(identifier);
        if (identifier == "separator") {
            listToolbarUsed->addItem(createToolbarSeparatorItem());
        } else if (identifier == "spacer") {
            listToolbarUsed->addItem(createToolbarSpacerItem());
        } else {
            const QAction *action = app->toolbar.value(identifier);
            if (action) {
                QListWidgetItem *item = new QListWidgetItem(action->icon(), action->text().remove('&'));
                item->setData(PoolListWidget::IdentifierRole, identifier);
                item->setData(PoolListWidget::ReusableRole, false);
                listToolbarUsed->addItem(item);
            }
        }
    }

    listToolbarUnused->clear();
    QMapIterator<QString, QAction *> it(unusedToolbarActions);
    while (it.hasNext()) {
        it.next();
        const QString identifier = it.key();
        const QAction *action = it.value();
        QListWidgetItem *item = new QListWidgetItem(action->icon(), action->text().remove('&'));
        item->setData(PoolListWidget::IdentifierRole, identifier);
        listToolbarUnused->addItem(item, false);
    }
    listToolbarUnused->addItem(createToolbarSeparatorItem());
    listToolbarUnused->addItem(createToolbarSpacerItem());
}

void SettingsDialog::save()
{
    // General

    app->settings->setAutoUpdates(checkboxUpdates->isChecked());
    app->setLanguage(comboLanguages->currentData().toString());
    app->recent->setLimit(spinboxRecent->value());

    // Repacking

    app->settings->setApktoolPath(fileboxApktool->getCurrentPath());
    app->settings->setOutputDirectory(fileboxOutput->getCurrentPath());
    app->settings->setFrameworksDirectory(fileboxFrameworks->getCurrentPath());
    app->settings->setDecompileSources(checkboxSources->isChecked());

    // Signing

    app->settings->setSignApk(groupSign->isChecked());
    app->settings->setApksignerPath(fileboxApksigner->getCurrentPath());

    // Optimizing

    app->settings->setOptimizeApk(groupZipalign->isChecked());
    app->settings->setZipalignPath(fileboxZipalign->getCurrentPath());

    // Installing

    app->settings->setAdbPath(fileboxAdb->getCurrentPath());

    // Toolbar

    QStringList toolbar;
    for (int i = 0; i < listToolbarUsed->count(); ++i) {
        const QString identifier = listToolbarUsed->item(i)->data(PoolListWidget::IdentifierRole).toString();
        toolbar.append(identifier);
    }
    app->settings->setToolbar(toolbar);
}

QListWidgetItem *SettingsDialog::createToolbarSeparatorItem() const
{
    QIcon icon = app->loadIcon("separator.png");
    //: Separator is a toolbar element which divides buttons with a vertical line.
    QListWidgetItem *separator = new QListWidgetItem(icon, tr("Separator"));
    separator->setData(PoolListWidget::IdentifierRole, "separator");
    separator->setData(PoolListWidget::ReusableRole, true);
    return separator;
}

QListWidgetItem *SettingsDialog::createToolbarSpacerItem() const
{
    QIcon icon = app->loadIcon("spacer.png");
    //: Spacer is a toolbar element which divides buttons with an empty space.
    QListWidgetItem *spacer = new QListWidgetItem(icon, tr("Spacer"));
    spacer->setData(PoolListWidget::IdentifierRole, "spacer");
    spacer->setData(PoolListWidget::ReusableRole, true);
    return spacer;
}
