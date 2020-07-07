#include "windows/optionsdialog.h"
#include "windows/devicemanager.h"
#include "windows/keymanager.h"
#include "widgets/toolbar.h"
#include "tools/adb.h"
#include "tools/apksigner.h"
#include "tools/apktool.h"
#include "tools/zipalign.h"
#include "base/application.h"
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

#ifdef Q_OS_WIN
    #include <QMessageBox>
#endif

#ifdef QT_DEBUG
    #include <QDebug>
#endif

OptionsDialog::OptionsDialog(QWidget *parent) : QDialog(parent)
{
    widget = nullptr;
    layout = new QVBoxLayout(this);
    initialize();
}

void OptionsDialog::addPage(const QString &title, QLayout *page, bool stretch)
{
    QWidget *container = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    QLabel *titleLabel = new QLabel(QString("%1").arg(title), this);
    QFrame *titleLine = new QFrame(this);
    titleLine->setFrameShape(QFrame::HLine);
    titleLine->setFrameShadow(QFrame::Sunken);
    QFont titleFont = titleLabel->font();
#ifndef Q_OS_OSX
    titleFont.setPointSize(13);
#else
    titleFont.setPointSize(17);
#endif
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

void OptionsDialog::load()
{
    // General

    checkboxSingleInstance->setChecked(app->settings->getSingleInstance());
    checkboxUpdates->setChecked(app->settings->getAutoUpdates());
    spinboxRecent->setValue(app->settings->getRecentLimit());
#ifdef Q_OS_WIN
    groupAssociate->setChecked(app->settings->getFileAssociation());
    checkboxExplorerOpen->setChecked(app->settings->getExplorerOpenIntegration());
    checkboxExplorerInstall->setChecked(app->settings->getExplorerInstallIntegration());
    checkboxExplorerOptimize->setChecked(app->settings->getExplorerOptimizeIntegration());
    checkboxExplorerSign->setChecked(app->settings->getExplorerSignIntegration());
#endif

    // Java

    fileboxJava->setCurrentPath(app->settings->getJavaPath());
    spinboxMinHeapSize->setValue(app->settings->getJavaMinHeapSize());
    spinboxMaxHeapSize->setValue(app->settings->getJavaMaxHeapSize());

    // Languages

    QList<Language> languages = app->getLanguages();
    comboLanguages->clear();
    const QString currentLocale = app->settings->getLanguage();
    for (const Language &language : languages) {
        const QPixmap flag = language.getFlag();
        const QString title = language.getTitle();
        const QString code = language.getCode();
        comboLanguages->addItem(flag, title, code);
        if (code == currentLocale) {
            comboLanguages->setCurrentIndex(comboLanguages->count() - 1);
        }
    }
    comboLanguages->setCurrentText(app->settings->getLanguage());

    // Repacking

    fileboxApktool->setCurrentPath(app->settings->getApktoolPath());
    fileboxOutput->setCurrentPath(app->settings->getOutputDirectory());
    fileboxFrameworks->setCurrentPath(app->settings->getFrameworksDirectory());
    checkboxAapt->setChecked(app->settings->getUseAapt2());
    checkboxSources->setChecked(app->settings->getDecompileSources());
    checkboxBrokenResources->setChecked(app->settings->getKeepBrokenResources());

    // Signing

    groupSign->setChecked(app->settings->getSignApk());
    fileboxApksigner->setCurrentPath(app->settings->getApksignerPath());

    // Optimizing

    groupZipalign->setChecked(app->settings->getOptimizeApk());
    fileboxZipalign->setCurrentPath(app->settings->getZipalignPath());

    // Installing

    fileboxAdb->setCurrentPath(app->settings->getAdbPath());

    emit loaded();
}

void OptionsDialog::save()
{
    // General

    app->settings->setSingleInstance(checkboxSingleInstance->isChecked());
    app->settings->setAutoUpdates(checkboxUpdates->isChecked());
    app->setLanguage(comboLanguages->currentData().toString());
    app->recent->setLimit(spinboxRecent->value());
#ifdef Q_OS_WIN
    bool integrationSuccess =
        app->settings->setFileAssociation(groupAssociate->isChecked()) &&
        app->settings->setExplorerOpenIntegration(checkboxExplorerOpen->isChecked()) &&
        app->settings->setExplorerInstallIntegration(checkboxExplorerInstall->isChecked()) &&
        app->settings->setExplorerOptimizeIntegration(checkboxExplorerOptimize->isChecked()) &&
        app->settings->setExplorerSignIntegration(checkboxExplorerSign->isChecked());
    if (!integrationSuccess) {
        QMessageBox::warning(this, QString(), tr("Could not register file association."));
    }
#endif

    // Java

    app->settings->setJavaPath(fileboxJava->getCurrentPath());
    app->settings->setJavaMinHeapSize(spinboxMinHeapSize->value());
    app->settings->setJavaMaxHeapSize(spinboxMaxHeapSize->value());

    // Repacking

    app->settings->setApktoolPath(fileboxApktool->getCurrentPath());
    app->settings->setOutputDirectory(fileboxOutput->getCurrentPath());
    app->settings->setFrameworksDirectory(fileboxFrameworks->getCurrentPath());
    app->settings->setUseAapt2(checkboxAapt->isChecked());
    app->settings->setDecompileSources(checkboxSources->isChecked());
    app->settings->setKeepBrokenResources(checkboxBrokenResources->isChecked());

    // Signing

    app->settings->setSignApk(groupSign->isChecked());
    app->settings->setApksignerPath(fileboxApksigner->getCurrentPath());

    // Optimizing

    app->settings->setOptimizeApk(groupZipalign->isChecked());
    app->settings->setZipalignPath(fileboxZipalign->getCurrentPath());

    // Installing

    app->settings->setAdbPath(fileboxAdb->getCurrentPath());

    emit saved();
}

void OptionsDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
#ifdef QT_DEBUG
        qDebug() << "TODO Unwanted behavior: this event is fired twice";
#endif
        const int currentPage = pageList->currentRow();
        initialize();
        pageList->setCurrentRow(currentPage);
    }
    QDialog::changeEvent(event);
}

void OptionsDialog::initialize()
{
    // Clear layout:

    if (widget) {
        delete widget;
    }

    widget = new QWidget(this);
    layout->addWidget(widget);

    setWindowTitle(tr("Options"));
    setWindowIcon(app->icons.get("settings.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(app->scale(800, 400));

    // General

    QFormLayout *pageGeneral = new QFormLayout;
    checkboxSingleInstance = new QCheckBox(tr("Single-window mode"), this);
    checkboxUpdates = new QCheckBox(tr("Check for updates automatically"), this);
    comboLanguages = new QComboBox(this);
    spinboxRecent = new QSpinBox(this);
    spinboxRecent->setMinimum(0);
    spinboxRecent->setMaximum(50);
    pageGeneral->addRow(checkboxSingleInstance);
    pageGeneral->addRow(checkboxUpdates);
    pageGeneral->addRow(tr("Language:"), comboLanguages);
    pageGeneral->addRow(tr("Maximum recent files:"), spinboxRecent);

#ifdef Q_OS_WIN
    groupAssociate = new QGroupBox(tr("Use APK Editor Studio for .apk files"), this);
    groupAssociate->setCheckable(true);
    checkboxExplorerOpen = new QCheckBox(tr("Use APK Editor Studio to open .apk files"), this);
    checkboxExplorerOpen->setIcon(app->icons.get("open.png"));
    //: "%1" will be replaced with an action name (e.g., Install, Optimize, Sign, etc.).
    const QString strExplorerIntegration(tr("Add %1 action to Windows Explorer context menu"));
    checkboxExplorerInstall = new QCheckBox(strExplorerIntegration.arg(tr("Install")), this);
    checkboxExplorerInstall->setIcon(app->icons.get("install.png"));
    checkboxExplorerOptimize = new QCheckBox(strExplorerIntegration.arg(tr("Optimize")), this);
    checkboxExplorerOptimize->setIcon(app->icons.get("optimize.png"));
    checkboxExplorerSign = new QCheckBox(strExplorerIntegration.arg(tr("Sign")), this);
    checkboxExplorerSign->setIcon(app->icons.get("key.png"));
    connect(groupAssociate, &QGroupBox::clicked, this, [this](bool checked) {
        checkboxExplorerOpen->setChecked(checked);
        checkboxExplorerInstall->setChecked(checked);
        checkboxExplorerOptimize->setChecked(checked);
        checkboxExplorerSign->setChecked(checked);
    });
    auto layoutAssocate = new QVBoxLayout(groupAssociate);
    layoutAssocate->addWidget(checkboxExplorerOpen);
    layoutAssocate->addWidget(checkboxExplorerInstall);
    layoutAssocate->addWidget(checkboxExplorerOptimize);
    layoutAssocate->addWidget(checkboxExplorerSign);
    pageGeneral->addRow(groupAssociate);
#endif

    // Java

    QFormLayout *pageJava = new QFormLayout;
    fileboxJava = new FileBox(true, this);
    fileboxJava->setDefaultPath("");
    const QString javaPath = app->getJavaPath();
    fileboxJava->setPlaceholderText(!javaPath.isEmpty() ? javaPath : tr("Extracted from environment variables by default"));
    spinboxMinHeapSize = new QSpinBox(this);
    spinboxMaxHeapSize = new QSpinBox(this);
    //: Megabytes
    const QString heapSizeSuffix = QString(" %1").arg(tr("MB"));
    spinboxMinHeapSize->setSuffix(heapSizeSuffix);
    spinboxMaxHeapSize->setSuffix(heapSizeSuffix);
    spinboxMinHeapSize->setSpecialValueText(tr("Default"));
    spinboxMaxHeapSize->setSpecialValueText(tr("Default"));
    spinboxMinHeapSize->setRange(0, std::numeric_limits<int>::max());
    spinboxMaxHeapSize->setRange(0, std::numeric_limits<int>::max());
    pageJava->addRow(tr("Java path:"), fileboxJava);
    //: "Heap" refers to a memory heap.
    pageJava->addRow(tr("Initial heap size:"), spinboxMinHeapSize);
    //: "Heap" refers to a memory heap.
    pageJava->addRow(tr("Maximum heap size:"), spinboxMaxHeapSize);

    // Repacking

    QFormLayout *pageRepack = new QFormLayout;
    fileboxApktool = new FileBox(false, this);
    fileboxApktool->setDefaultPath("");
    fileboxApktool->setPlaceholderText(Apktool::getDefaultPath());
    fileboxOutput = new FileBox(true, this);
    fileboxOutput->setDefaultPath("");
    fileboxOutput->setPlaceholderText(Apktool::getDefaultOutputPath());
    fileboxFrameworks = new FileBox(true, this);
    fileboxFrameworks->setDefaultPath("");
    fileboxFrameworks->setPlaceholderText(Apktool::getDefaultFrameworksPath());
    checkboxAapt = new QCheckBox("AAPT2", this);
    //: "Smali" is the name of the tool/format, don't translate it.
    checkboxSources = new QCheckBox(tr("Decompile source code (smali)"), this);
    checkboxBrokenResources = new QCheckBox(tr("Decompile broken resources"), this);
    //: "Apktool" is the name of the tool, don't translate it.
    pageRepack->addRow(tr("Apktool path:"), fileboxApktool);
    pageRepack->addRow(tr("Extraction path:"), fileboxOutput);
    pageRepack->addRow(tr("Frameworks path:"), fileboxFrameworks);
    pageRepack->addRow(checkboxAapt);
    pageRepack->addRow(checkboxSources);
    pageRepack->addRow(checkboxBrokenResources);
    pageRepack->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // Signing

    QVBoxLayout *pageSign = new QVBoxLayout;
    groupSign = new QGroupBox(tr("Enable"), this);
    groupSign->setCheckable(true);
    fileboxApksigner = new FileBox(false, this);
    fileboxApksigner->setDefaultPath("");
    fileboxApksigner->setPlaceholderText(Apksigner::getDefaultPath());
    QFormLayout *layoutSign = new QFormLayout(groupSign);
    //: This string refers to multiple keys (as in "Manager of keys").
    QPushButton *btnKeyManager = new QPushButton(tr("Open Key Manager"), this);
    btnKeyManager->setIcon(app->icons.get("key.png"));
    btnKeyManager->setMinimumHeight(app->scale(30));
    connect(btnKeyManager, &QPushButton::clicked, this, [this]() {
        KeyManager keyManager(this);
        keyManager.exec();
    });
    //: "Apksigner" is the name of the tool, don't translate it.
    layoutSign->addRow(tr("Apksigner path:"), fileboxApksigner);
    layoutSign->addRow(btnKeyManager);
    layoutSign->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    pageSign->addWidget(groupSign);

    // Optimizing

    QVBoxLayout *pageZipalign = new QVBoxLayout;
    groupZipalign = new QGroupBox(tr("Enable"), this);
    groupZipalign->setCheckable(true);
    fileboxZipalign = new FileBox(false, this);
    fileboxZipalign->setDefaultPath("");
    fileboxZipalign->setPlaceholderText(Zipalign::getDefaultPath());
    QFormLayout *layoutZipalign = new QFormLayout(groupZipalign);
    //: "Zipalign" is the name of the tool, don't translate it.
    layoutZipalign->addRow(tr("Zipalign path:"), fileboxZipalign);
    layoutZipalign->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    pageZipalign->addWidget(groupZipalign);

    // Installing

    QFormLayout *pageInstall = new QFormLayout;
    fileboxAdb = new FileBox(false, this);
    fileboxAdb->setDefaultPath("");
    fileboxAdb->setPlaceholderText(Adb::getDefaultPath());
    //: This string refers to multiple devices (as in "Manager of devices").
    QPushButton *btnDeviceManager = new QPushButton(tr("Open Device Manager"), this);
    btnDeviceManager->setIcon(app->icons.get("devices.png"));
    btnDeviceManager->setMinimumHeight(app->scale(30));
    connect(btnDeviceManager, &QPushButton::clicked, this, [this]() {
        DeviceManager deviceManager(this);
        deviceManager.exec();
    });
    //: "ADB" is the name of the tool, don't translate it.
    pageInstall->addRow(tr("ADB path:"), fileboxAdb);
    pageInstall->addRow(btnDeviceManager);
    pageInstall->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // Initialize

    pageStack = new QStackedWidget(this);
    pageStack->setFrameShape(QFrame::StyledPanel);
    pageList = new QListWidget(this);
    addPage(tr("General"), pageGeneral);
    addPage("Java", pageJava);
    addPage(tr("Repacking"), pageRepack);
    addPage(tr("Signing APK"), pageSign);
    addPage(tr("Optimizing APK"), pageZipalign);
    addPage(tr("Installing APK"), pageInstall);
    pageList->setCurrentRow(0);
    pageList->setMaximumWidth(pageList->sizeHintForColumn(0) + 60);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    QPushButton *btnApply = buttons->button(QDialogButtonBox::Apply);

    QGridLayout *layoutPages = new QGridLayout(widget);
    layoutPages->addWidget(pageList, 0, 0);
    layoutPages->addWidget(pageStack, 0, 1);
    layoutPages->addWidget(buttons, 1, 0, 1, 2);

    load();

    connect(pageList, &QListWidget::currentRowChanged, pageStack, &QStackedWidget::setCurrentIndex);
    connect(buttons, &QDialogButtonBox::accepted, this, &OptionsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &OptionsDialog::reject);
    connect(btnApply, &QPushButton::clicked, this, &OptionsDialog::save);
    connect(this, &OptionsDialog::accepted, this, &OptionsDialog::save);
}
