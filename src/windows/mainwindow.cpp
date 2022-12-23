#include <QCommandLineParser>
#include "windows/mainwindow.h"
#include "windows/aboutdialog.h"
#include "windows/dialogs.h"
#include "windows/updatedialog.h"
#include "widgets/projectmanager.h"
#include "widgets/filesystemtree.h"
#include "widgets/iconlist.h"
#include "widgets/logview.h"
#include "widgets/manifestview.h"
#include "widgets/packagelist.h"
#include "widgets/resourceabstractview.h"
#include "widgets/resourcetree.h"
#include "widgets/toolbar.h"
#include "sheets/basefilesheet.h"
#include "sheets/welcomesheet.h"
#include "tools/keystore.h"
#include "base/application.h"
#include "base/extralistitemproxy.h"
#include "base/settings.h"
#include "apk/package.h"
#include "apk/project.h"
#include <QBoxLayout>
#include <QDebug>
#include <QDockWidget>
#include <QDropEvent>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenuBar>
#include <QMimeData>
#include <QMimeDatabase>
#include <QTimer>

int MainWindow::instances = 0;

MainWindow::MainWindow(PackageListModel &packages, QWidget *parent)
    : QMainWindow(parent)
    , packages(packages)
{
    ++instances;

    setAttribute(Qt::WA_DeleteOnClose);
    setAcceptDrops(true);

    projectManager = new ProjectManager(packages, this);
    connect(projectManager, &ProjectManager::currentPackageStateChanged, this, &MainWindow::updateWindowForPackage);
    connect(projectManager, &ProjectManager::projectCreated, this, [this](Project *project) {
        setCurrentPackage(project->getPackage());
    });

    initMenus();
    initWidgets();

    connect(app->settings, &Settings::resetDone, this, [this]() {
        restoreGeometry(QByteArray());
        setInitialSize();
        restoreState(defaultState);
        toolbar->initialize(app->settings->getMainWindowToolbar());
    });

    QEvent languageChangeEvent(QEvent::LanguageChange);
    app->sendEvent(this, &languageChangeEvent);

    QTimer::singleShot(1000, this, [this]() {
        checkToolsAvailable();
        if (app->settings->getAutoUpdates()) {
            updateDialog->checkUpdates();
        }
    });

    qDebug();
}

MainWindow::~MainWindow()
{
    --instances;
}

void MainWindow::openApk(const QString &path)
{
    if (auto package = addPackage(path)) {
        auto command = package->createCommandChain();
        command->add(package->createUnpackCommand(), true);
        command->run();
    }
}

void MainWindow::openExternalApk()
{
    const QStringList paths = Dialogs::getOpenApkFilenames(this);
    for (const QString &path : paths) {
        openApk(path);
    }
}

void MainWindow::optimizeExternalApk()
{
    const QStringList paths = Dialogs::getOpenApkFilenames(this);
    for (const QString &path : paths) {
        if (auto package = addPackage(path)) {
            auto command = package->createCommandChain();
            command->add(package->createZipalignCommand(), true);
            command->run();
        }
    }
}

void MainWindow::signExternalApk()
{
    const auto keystore = Keystore::get(this);
    if (!keystore) {
        return;
    }
    const QStringList paths = Dialogs::getOpenApkFilenames(this);
    for (const QString &path : paths) {
        if (auto package = addPackage(path)) {
            auto command = package->createCommandChain();
            command->add(package->createSignCommand(keystore.get()), true);
            command->run();
        }
    }
}

void MainWindow::installExternalApk()
{
    const auto device = Dialogs::getInstallDevice(this);
    if (device.isNull()) {
        return;
    }
    const QStringList paths = Dialogs::getOpenApkFilenames(this);
    for (const QString &path : paths) {
        if (auto package = addPackage(path)) {
            auto command = package->createCommandChain();
            command->add(package->createInstallCommand(device.getSerial()), true);
            command->run();
        }
    }
}

void MainWindow::processArguments(const QStringList &arguments)
{
    QCommandLineParser cli;
    QCommandLineOption optimizeOption(QStringList{"o", "optimize", "z", "zipalign"});
    QCommandLineOption signOption(QStringList{"s", "sign"});
    QCommandLineOption installOption(QStringList{"i", "install"});
    cli.addOption(optimizeOption);
    cli.addOption(signOption);
    cli.addOption(installOption);
    cli.parse(arguments);

    const auto positionalArguments = cli.positionalArguments();
    for (const QString &path : positionalArguments) {
        if (auto package = addPackage(path)) {
            auto command = package->createCommandChain();
            if (!cli.isSet(optimizeOption) && !cli.isSet(signOption) && !cli.isSet(installOption)) {
                command->add(package->createUnpackCommand(), true);
            } else {
                if (cli.isSet(optimizeOption)) {
                    command->add(package->createZipalignCommand(), true);
                }
                if (cli.isSet(signOption)) {
                    auto keystore = Keystore::get(this);
                    if (keystore) {
                        command->add(package->createSignCommand(keystore.get()), true);
                    }
                }
                if (cli.isSet(installOption)) {
                    const auto device = Dialogs::getInstallDevice(this);
                    if (!device.isNull()) {
                        command->add(package->createInstallCommand(device.getSerial()), true);
                    }
                }
            }
            command->run();
        }
    }
}

void MainWindow::setCurrentPackage(Package *package)
{
    packageList->setCurrentPackage(package);
}

void MainWindow::setInitialSize()
{
    resize(Utils::scale(1200, 700));
}

void MainWindow::initWidgets()
{
    qDebug() << "Initializing widgets...";

    setInitialSize();

    projectManager->setDefaultWidget(new WelcomeSheet(this));
    setCentralWidget(projectManager);

    resourceTree = new ResourceAbstractView(new ResourceTree, this);
    resourceTree->setModel(dummyResourceModel = new ResourceItemsModel(this)); // Always display header
    resourceTree->getView<ResourceTree *>()->header()->restoreState(app->settings->getResourceTreeHeader());
    connect(resourceTree, &ResourceAbstractView::editRequested, this, [this](const ResourceModelIndex &index) {
        getCurrentProject()->openResourceTab(index);
    });
    resourceFilterInput = new QLineEdit(this);
    resourceFilterInput->setClearButtonEnabled(true);
    connect(resourceFilterInput, &QLineEdit::textChanged,
            resourceTree->getView<ResourceTree *>(), &ResourceTree::setFilter);
    auto dockResourceWidget = new QWidget(this);
    auto resourceLayout = new QVBoxLayout(dockResourceWidget);
    resourceLayout->addWidget(resourceTree);
    resourceLayout->addWidget(resourceFilterInput);
    resourceLayout->setMargin(0);
    resourceLayout->setSpacing(2);

    filesystemTree = new ResourceAbstractView(new FileSystemTree, this);
    filesystemTree->setModel(dummyFileSystemModel = new FileSystemModel(this)); // Always display header
    filesystemTree->getView<FileSystemTree *>()->header()->restoreState(app->settings->getFileSystemTreeHeader());
    connect(filesystemTree, &ResourceAbstractView::editRequested, this, [this](const ResourceModelIndex &index) {
        getCurrentProject()->openResourceTab(index);
    });
    auto dockFilesystemWidget = new QWidget(this);
    auto filesystemLayout = new QVBoxLayout(dockFilesystemWidget);
    filesystemLayout->addWidget(filesystemTree);
    filesystemLayout->setMargin(0);

    iconList = new ResourceAbstractView(new IconList, this);
    connect(iconList, &ResourceAbstractView::editRequested, this, [this](const ResourceModelIndex &index) {
        getCurrentProject()->openResourceTab(index);
    });
    auto dockIconsWidget = new QWidget(this);
    auto iconsLayout = new QVBoxLayout(dockIconsWidget);
    iconsLayout->addWidget(iconList);
    iconsLayout->setMargin(0);
    iconsLayout->setSpacing(1);

    manifestTable = new ManifestView(this);
    connect(manifestTable, &ManifestView::titleEditorRequested, this, [this]() {
        getCurrentProject()->openTitlesTab();
    });
    auto dockManifestWidget = new QWidget(this);
    auto manifestLayout = new QVBoxLayout(dockManifestWidget);
    manifestLayout->addWidget(manifestTable);
    manifestLayout->setMargin(0);

    logView = new LogView(this);
    packageList = new PackageList(this);
    connect(packageList, &PackageList::currentPackageChanged, this, &MainWindow::onPackageSwitched);
    welcomeItemProxy = new ExtraListItemProxy(this);
    welcomeItemProxy->setSourceModel(&packages);
    welcomeItemProxy->prependRow();
    welcomeItemProxy->setData(welcomeItemProxy->index(0, 0), QIcon::fromTheme("apk-editor-studio"), Qt::DecorationRole);
    packageList->setModel(welcomeItemProxy);
    auto dockProjectsWidget = new QWidget(this);
    auto projectsLayout = new QVBoxLayout(dockProjectsWidget);
    projectsLayout->addWidget(packageList);
    projectsLayout->addWidget(logView);
    projectsLayout->setMargin(0);
    projectsLayout->setSpacing(1);

    dockProjects = new QDockWidget(this);
    dockResources = new QDockWidget(this);
    dockFilesystem = new QDockWidget(this);
    dockManifest = new QDockWidget(this);
    dockIcons = new QDockWidget(this);
    dockProjects->setObjectName("DockProjects");
    dockResources->setObjectName("DockResources");
    dockFilesystem->setObjectName("DockFilesystem");
    dockManifest->setObjectName("DockManifest");
    dockIcons->setObjectName("DockIcons");
    dockProjects->setWidget(dockProjectsWidget);
    dockResources->setWidget(dockResourceWidget);
    dockFilesystem->setWidget(dockFilesystemWidget);
    dockManifest->setWidget(dockManifestWidget);
    dockIcons->setWidget(dockIconsWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockProjects);
    addDockWidget(Qt::LeftDockWidgetArea, dockResources);
    addDockWidget(Qt::LeftDockWidgetArea, dockFilesystem);
    addDockWidget(Qt::RightDockWidgetArea, dockManifest);
    addDockWidget(Qt::RightDockWidgetArea, dockIcons);
    tabifyDockWidget(dockResources, dockFilesystem);
    dockResources->raise();

    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    defaultState = saveState();

    restoreGeometry(app->settings->getMainWindowGeometry());
    restoreState(app->settings->getMainWindowState());
}

void MainWindow::initMenus()
{
    qDebug() << "Initializing menus...";

    // File Menu:

    auto actionOpenApk = app->actions.getOpenApk(this);
    connect(actionOpenApk, &QAction::triggered, this, &MainWindow::openExternalApk);
    auto actionSaveApk = projectManager->getActionSavePackage();
    auto actionInstallApk = projectManager->getActionInstallPackage();
    auto actionInstallExternal = app->actions.getInstallApk(this);
    connect(actionInstallExternal, &QAction::triggered, this, &MainWindow::installExternalApk);
    auto actionOptimizeExternal = app->actions.getOptimizeApk(this);
    connect(actionOptimizeExternal, &QAction::triggered, this, &MainWindow::optimizeExternalApk);
    auto actionSignExternal = app->actions.getSignApk(this);
    connect(actionSignExternal, &QAction::triggered, this, &MainWindow::signExternalApk);
    auto actionExploreApk = projectManager->getActionExplorePackage();
    auto actionCloseApk = projectManager->getActionClosePackage();

    // Recent Menu:

    menuRecent = new QMenu(this);
    menuRecent->setIcon(QIcon::fromTheme("document-recent"));
    actionRecentClear = new QAction(QIcon::fromTheme("edit-delete"), QString(), this);
    connect(actionRecentClear, &QAction::triggered, app->settings, &Settings::clearRecentApkList);
    actionRecentNone = new QAction(this);
    actionRecentNone->setEnabled(false);
    connect(app->settings, &Settings::recentApkListUpdated, this, &MainWindow::updateRecentMenu);
    updateRecentMenu();

    // Tools Menu:

    auto actionKeyManager = app->actions.getOpenKeyManager(this);
    auto actionDeviceManager = app->actions.getOpenDeviceManager(this);
    auto actionAndroidExplorer = app->actions.getOpenAndroidExplorer(this);
    auto actionScreenshot = app->actions.getTakeScreenshot(this);
    auto actionFrameworkManager = app->actions.getOpenFrameworkManager(this);
    auto actionProjectPage = projectManager->getActionOpenProjectPage();
    auto actionSearchInProject = projectManager->getActionSearch();
    auto actionTitleEditor = projectManager->getActionEditTitles();
    auto actionPermissionEditor = projectManager->getActionEditPermissions();
    auto actionClonePackage = projectManager->getActionClonePackage();
    auto actionViewSignatures = projectManager->getActionViewSignatures();

    // Settings Menu:

    auto actionOptions = app->actions.getOpenOptions(this);

    // Window Menu:

    actionNewWindow = new QAction(this);
    actionNewWindow->setIcon(QIcon::fromTheme("window-new"));
    actionNewWindow->setShortcut(QKeySequence("Ctrl+N"));
    connect(actionNewWindow, &QAction::triggered, app, &Application::createNewInstance);

    // Help Menu:

    auto actionDonate = app->actions.getVisitDonatePage();

    updateDialog = new UpdateDialog(this);
    actionCheckUpdates = new QAction(QIcon::fromTheme("help-update"), QString(), this);
    connect(actionCheckUpdates, &QAction::triggered, this, [this]() {
        updateDialog->checkUpdates();
        updateDialog->open();
    });

    actionAbout = new QAction(QIcon::fromTheme("apk-editor-studio"), QString(), this);
    actionAbout->setMenuRole(QAction::AboutRole);
    connect(actionAbout, &QAction::triggered, this, [this]() {
        AboutDialog about(this);
        about.exec();
    });

    actionAboutQt = new QAction(QIcon::fromTheme("qt"), QString(), this);
    actionAboutQt->setMenuRole(QAction::AboutQtRole);
    connect(actionAboutQt, &QAction::triggered, app, &Application::aboutQt);

    // Menu Bar:

    menuFile = menuBar()->addMenu(QString());
    menuFile->addAction(actionOpenApk);
    menuFile->addMenu(menuRecent);
    menuFile->addSeparator();
    menuFile->addAction(actionSaveApk);
    menuFile->addSeparator();
    menuFile->addAction(actionInstallApk);
    menuFile->addSeparator();
    menuFile->addAction(actionInstallExternal);
    menuFile->addAction(actionOptimizeExternal);
    menuFile->addAction(actionSignExternal);
    menuFile->addSeparator();
    menuFile->addAction(actionExploreApk);
    menuFile->addSeparator();
    menuFile->addAction(actionCloseApk);
    menuFile->addSeparator();
    menuFile->addAction(app->actions.getExit(this));
    menuBar()->addMenu(projectManager->getTabMenu());
    menuTools = menuBar()->addMenu(QString());
    menuTools->addAction(actionKeyManager);
    menuTools->addSeparator();
    menuTools->addAction(actionDeviceManager);
    menuTools->addAction(actionAndroidExplorer);
    menuTools->addAction(actionScreenshot);
    menuTools->addSeparator();
    menuTools->addAction(actionFrameworkManager);
    menuTools->addSeparator();
    menuTools->addAction(actionProjectPage);
    menuTools->addAction(actionSearchInProject);
    menuTools->addSeparator();
    menuTools->addAction(actionTitleEditor);
    menuTools->addAction(actionPermissionEditor);
    menuTools->addAction(actionClonePackage);
    menuTools->addSeparator();
    menuTools->addAction(actionViewSignatures);
    menuSettings = menuBar()->addMenu(QString());
    menuSettings->addAction(actionOptions);
    menuSettings->addSeparator();
    menuSettings->addMenu(app->actions.getLanguages());
    menuSettings->addSeparator();
    menuSettings->addAction(app->actions.getResetSettings(this));
    menuWindow = menuBar()->addMenu(QString());
    menuHelp = menuBar()->addMenu(QString());
    menuHelp->addAction(app->actions.getVisitWebPage());
    menuHelp->addAction(app->actions.getVisitSourcePage());
    menuHelp->addAction(actionDonate);
    menuHelp->addSeparator();
    menuHelp->addAction(actionCheckUpdates);
    menuHelp->addSeparator();
    menuHelp->addAction(actionAboutQt);
    menuHelp->addAction(actionAbout);

    // Tool Bar:

    auto actionSaveFile = projectManager->getActionSaveFile();
    auto actionSaveFileAs = projectManager->getActionSaveFileAs();

    toolbar = new Toolbar(this);
    toolbar->setObjectName("Toolbar");
    toolbar->addActionToPool("open-project", actionOpenApk);
    toolbar->addActionToPool("save-project", actionSaveApk);
    toolbar->addActionToPool("install-project", actionInstallApk);
    toolbar->addActionToPool("open-contents", actionExploreApk);
    toolbar->addActionToPool("close-project", actionCloseApk);
    toolbar->addActionToPool("save", actionSaveFile);
    toolbar->addActionToPool("save-as", actionSaveFileAs);
    toolbar->addActionToPool("project-manager", actionProjectPage);
    toolbar->addActionToPool("search-project", actionSearchInProject);
    toolbar->addActionToPool("title-editor", actionTitleEditor);
    toolbar->addActionToPool("permission-editor", actionPermissionEditor);
    toolbar->addActionToPool("rename-package", actionClonePackage);
    toolbar->addActionToPool("view-signatures", actionViewSignatures);
    toolbar->addActionToPool("device-manager", actionDeviceManager);
    toolbar->addActionToPool("android-explorer", actionAndroidExplorer);
    toolbar->addActionToPool("screenshot", actionScreenshot);
    toolbar->addActionToPool("key-manager", actionKeyManager);
    toolbar->addActionToPool("framework-manager", actionFrameworkManager);
    toolbar->addActionToPool("new-window", actionNewWindow);
    toolbar->addActionToPool("settings", actionOptions);
    toolbar->addActionToPool("check-updates", actionCheckUpdates);
    toolbar->addActionToPool("donate", actionDonate);
    toolbar->initialize(app->settings->getMainWindowToolbar());
    addToolBar(toolbar);
    connect(toolbar, &Toolbar::updated, app->settings, &Settings::setMainWindowToolbar);
}

void MainWindow::checkToolsAvailable()
{
    //: "%1" will be replaced with a tool name.
    const QString question(tr("%1 not found. Restore the default path?"));

    const auto apktoolPath = Utils::toAbsolutePath(app->settings->getApktoolPath());
    if (!apktoolPath.isEmpty() && !QFile::exists(apktoolPath)) {
        if (QMessageBox::question(this, {}, question.arg("Apktool")) == QMessageBox::Yes) {
            app->settings->setApktoolPath({});
        }
    }
}

void MainWindow::retranslate()
{
    tr("Remove Temporary Files..."); // TODO For future use

    // Docks:

    dockProjects->setWindowTitle(tr("Projects"));
    dockResources->setWindowTitle(tr("Resources"));
    dockFilesystem->setWindowTitle(tr("File System"));
    dockManifest->setWindowTitle(tr("Manifest"));
    dockIcons->setWindowTitle(tr("Icons"));
    resourceFilterInput->setPlaceholderText(tr("Filter"));

    // Menu Bar:

    //: Refers to a menu bar (along with Edit, View, Window, Help, and similar items).
    menuFile->setTitle(tr("&File"));
    //: Refers to a menu bar (along with File, Edit, View, Window, Help, and similar items).
    menuTools->setTitle(tr("&Tools"));
    //: Refers to a menu bar (along with File, Edit, View, Window, Help, and similar items).
    menuSettings->setTitle(tr("&Settings"));
    //: Refers to a menu bar (along with File, Edit, View, Help, and similar items).
    menuWindow->setTitle(tr("&Window"));
    //: Refers to a menu bar (along with File, Edit, View, Window, and similar items).
    menuHelp->setTitle(tr("&Help"));

    // Recent Menu:

    menuRecent->setTitle(tr("Open &Recent"));
    actionRecentClear->setText(tr("&Clear List"));
    actionRecentNone->setText(tr("No Recent Files"));

    // Window Menu:

    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionNewWindow->setText(tr("Open &New Window"));
    menuWindow->clear();
    menuWindow->addAction(actionNewWindow);
    menuWindow->addSeparator();
    menuWindow->addActions(createPopupMenu()->actions());

    // Help Menu:

    actionCheckUpdates->setText(app->translate("ActionProvider", "Check for &Updates"));
    //: Don't translate the "APK Editor Studio" part.
    actionAbout->setText(tr("&About APK Editor Studio..."));
    //: Don't translate the "&Qt" part.
    actionAboutQt->setText(tr("About &Qt..."));

    // Miscellaneous:

    toolbar->setWindowTitle(tr("Tools"));
    welcomeItemProxy->setData(welcomeItemProxy->index(0, 0), tr("Welcome"), Qt::DisplayRole);
}

Package *MainWindow::addPackage(const QString &path)
{
    if (auto existing = packages.existing(path)) {
        //: "%1" will be replaced with a path to an APK.
        const QString question = tr("This APK is already open:\n%1\nDo you want to reopen it and lose any unsaved changes?").arg(existing->getOriginalPath());
        const int answer = QMessageBox::question(this, QString(), question);
        if (answer != QMessageBox::Yes) {
            return nullptr;
        }
        packages.close(existing);
    }
    auto package = new Package(path);
    packages.add(package);
    return package;
}

void MainWindow::updateWindowForPackage(Package *package)
{
    if (package) {
        setWindowTitle(QString("%1[*]").arg(package->getOriginalPath()));
        setWindowModified(package->getState().isModified());
    } else {
        setWindowTitle(QString());
        setWindowModified(false);
    }
}

void MainWindow::updateRecentMenu()
{
    menuRecent->clear();
    auto recentList = app->settings->getRecentApkList();
    for (const RecentFile &recentEntry : recentList) {
        auto action = new QAction(recentEntry.thumbnail(), recentEntry.filename(), this);
        menuRecent->addAction(action);
        connect(action, &QAction::triggered, this, [=]() {
            openApk(recentEntry.filename());
        });
    }
    menuRecent->addSeparator();
    menuRecent->addAction(recentList.isEmpty() ? actionRecentNone : actionRecentClear);
}

void MainWindow::onPackageSwitched(Package *package)
{
    projectManager->setCurrentProject(package);

    resourceTree->setModel(package ? &package->resourcesModel : dummyResourceModel);
    filesystemTree->setModel(package ? &package->filesystemModel : dummyFileSystemModel);
    iconList->setModel(package ? &package->iconsProxy : nullptr);
    logView->setModel(package ? &package->logModel : nullptr);
    manifestTable->setModel(package ? &package->manifestModel : nullptr);

    updateWindowForPackage(package);
}

Project *MainWindow::getCurrentProject() const
{
    return projectManager->getCurrentProject();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
        const bool showRubberBand = mimeData->urls().constFirst().toString().toLower().endsWith(".apk");
        rubberBand->setGeometry(rect());
        rubberBand->setVisible(showRubberBand);
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    rubberBand->hide();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        const auto urls = mimeData->urls();
        for (const QUrl &url : urls) {
            const QString path = url.toLocalFile();
            const QMimeType mime = QMimeDatabase().mimeTypeForFile(path);
            if (mime.inherits("application/zip")) {
                openApk(path);
                event->acceptProposedAction();
            }
        }
    }
    rubberBand->hide();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (instances == 1) {
        Project *unsavedProject = projectManager->hasUnsavedProjects();
        if (unsavedProject) {
            setCurrentPackage(unsavedProject->getPackage());
            const QString question = tr("You have unsaved changes.\nDo you want to discard them and exit?");
            const int answer = QMessageBox::question(this, QString(), question, QMessageBox::Discard, QMessageBox::Cancel);
            if (answer != QMessageBox::Discard) {
                event->ignore();
                return;
            }
        }
    }
    app->settings->setMainWindowGeometry(saveGeometry());
    app->settings->setMainWindowState(saveState());
    app->settings->setResourceTreeHeader(resourceTree->getView<ResourceTree *>()->header()->saveState());
    app->settings->setFileSystemTreeHeader(filesystemTree->getView<FileSystemTree *>()->header()->saveState());
    event->accept();
}
