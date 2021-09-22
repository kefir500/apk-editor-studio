#include <QCommandLineParser>
#include "windows/mainwindow.h"
#include "windows/aboutdialog.h"
#include "windows/dialogs.h"
#include "windows/signatureviewer.h"
#include "widgets/centralwidget.h"
#include "widgets/filesystemtree.h"
#include "widgets/iconlist.h"
#include "widgets/logview.h"
#include "widgets/manifestview.h"
#include "widgets/projectlist.h"
#include "widgets/projectwidget.h"
#include "widgets/resourceabstractview.h"
#include "widgets/resourcetree.h"
#include "widgets/toolbar.h"
#include "sheets/basefilesheet.h"
#include "sheets/welcomesheet.h"
#include "tools/keystore.h"
#include "base/application.h"
#include "base/extralistitemproxy.h"
#include "base/settings.h"
#include "base/updater.h"
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

MainWindow::MainWindow(ProjectItemsModel &projects, QWidget *parent)
    : QMainWindow(parent)
    , projects(projects)
{
    ++instances;

    setAttribute(Qt::WA_DeleteOnClose);
    setAcceptDrops(true);

    onProjectAdded({}, 0, projects.rowCount() - 1);
    connect(&projects, &ProjectItemsModel::rowsInserted, this, &MainWindow::onProjectAdded);
    connect(&projects, &ProjectItemsModel::rowsAboutToBeRemoved, this, &MainWindow::onProjectAboutToBeRemoved);

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

    if (app->settings->getAutoUpdates()) {
        // Delay to prevent uninitialized window render
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [=]() {
            Updater::check(false, this);
            timer->deleteLater();
        });
        timer->setSingleShot(true);
        timer->start(1000);
    }

    qDebug();
}

MainWindow::~MainWindow()
{
    --instances;
}

void MainWindow::openApk(const QString &path)
{
    if (auto project = addProject(path)) {
        auto command = project->createCommandChain();
        command->add(project->createUnpackCommand(), true);
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
        if (auto project = addProject(path)) {
            auto command = project->createCommandChain();
            command->add(project->createZipalignCommand(), true);
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
        if (auto project = addProject(path)) {
            auto command = project->createCommandChain();
            command->add(project->createSignCommand(keystore.get()), true);
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
        if (auto project = addProject(path)) {
            auto command = project->createCommandChain();
            command->add(project->createInstallCommand(device.getSerial()), true);
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
        if (auto project = addProject(path)) {
            auto command = project->createCommandChain();
            if (!cli.isSet(optimizeOption) && !cli.isSet(signOption) && !cli.isSet(installOption)) {
                command->add(project->createUnpackCommand(), true);
            } else {
                if (cli.isSet(optimizeOption)) {
                    command->add(project->createZipalignCommand(), true);
                }
                if (cli.isSet(signOption)) {
                    auto keystore = Keystore::get(this);
                    if (keystore) {
                        command->add(project->createSignCommand(keystore.get()), true);
                    }
                }
                if (cli.isSet(installOption)) {
                    const auto device = Dialogs::getInstallDevice(this);
                    if (!device.isNull()) {
                        command->add(project->createInstallCommand(device.getSerial()), true);
                    }
                }
            }
            command->run();
        }
    }
}

void MainWindow::setCurrentProject(Project *project)
{
    projectList->setCurrentProject(project);
}

void MainWindow::setInitialSize()
{
    resize(Utils::scale(1200, 700));
}

void MainWindow::initWidgets()
{
    qDebug() << "Initializing widgets...";

    setInitialSize();

    centralWidget = new CentralWidget(this);
    setCentralWidget(centralWidget);

    resourceTree = new ResourceAbstractView(new ResourceTree, this);
    resourceTree->setModel(dummyResourceModel = new ResourceItemsModel(this)); // Always display header
    resourceTree->getView<ResourceTree *>()->header()->restoreState(app->settings->getResourceTreeHeader());
    connect(resourceTree, &ResourceAbstractView::editRequested, this, [this](const ResourceModelIndex &index) {
        getCurrentProjectWidget()->openResourceTab(index);
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
        getCurrentProjectWidget()->openResourceTab(index);
    });
    auto dockFilesystemWidget = new QWidget(this);
    auto filesystemLayout = new QVBoxLayout(dockFilesystemWidget);
    filesystemLayout->addWidget(filesystemTree);
    filesystemLayout->setMargin(0);

    iconList = new ResourceAbstractView(new IconList, this);
    connect(iconList, &ResourceAbstractView::editRequested, this, [this](const ResourceModelIndex &index) {
        getCurrentProjectWidget()->openResourceTab(index);
    });
    auto dockIconsWidget = new QWidget(this);
    auto iconsLayout = new QVBoxLayout(dockIconsWidget);
    iconsLayout->addWidget(iconList);
    iconsLayout->setMargin(0);
    iconsLayout->setSpacing(1);

    manifestTable = new ManifestView(this);
    connect(manifestTable, &ManifestView::titleEditorRequested, this, [this]() {
        getCurrentProjectWidget()->openTitlesTab();
    });
    auto dockManifestWidget = new QWidget(this);
    auto manifestLayout = new QVBoxLayout(dockManifestWidget);
    manifestLayout->addWidget(manifestTable);
    manifestLayout->setMargin(0);

    logView = new LogView(this);
    projectList = new ProjectList(this);
    connect(projectList, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]() {
        onProjectSwitched(getCurrentProject());
    });
    welcomeItemProxy = new ExtraListItemProxy(this);
    welcomeItemProxy->setSourceModel(&projects);
    welcomeItemProxy->prependRow();
    welcomeItemProxy->setData(welcomeItemProxy->index(0, 0), QIcon::fromTheme("apk-editor-studio"), Qt::DecorationRole);
    welcomePage = new WelcomeSheet(this);
    projectList->setModel(welcomeItemProxy);
    auto dockProjectsWidget = new QWidget(this);
    auto projectsLayout = new QVBoxLayout(dockProjectsWidget);
    projectsLayout->addWidget(projectList);
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

    auto actionApkOpen = app->actions.getOpenApk(this);
    connect(actionApkOpen, &QAction::triggered, this, &MainWindow::openExternalApk);
    actionApkSave = new QAction(QIcon::fromTheme("apk-save"), QString(), this);
    actionApkSave->setShortcut(QKeySequence("Ctrl+Alt+S"));
    actionApkInstall = new QAction(QIcon::fromTheme("apk-install"), QString(), this);
    actionApkInstall->setShortcut(QKeySequence("Ctrl+I"));
    auto actionInstallExternal = app->actions.getInstallApk(this);
    connect(actionInstallExternal, &QAction::triggered, this, &MainWindow::installExternalApk);
    auto actionOptimizeExternal = app->actions.getOptimizeApk(this);
    connect(actionOptimizeExternal, &QAction::triggered, this, &MainWindow::optimizeExternalApk);
    auto actionSignExternal = app->actions.getSignApk(this);
    connect(actionSignExternal, &QAction::triggered, this, &MainWindow::signExternalApk);
    actionApkExplore = new QAction(QIcon::fromTheme("folder-open"), QString(), this);
    actionApkExplore->setShortcut(QKeySequence("Ctrl+E"));
    actionApkClose = new QAction(QIcon::fromTheme("document-close"), QString(), this);
    actionApkClose->setShortcut(QKeySequence("Ctrl+W"));

    // Recent Menu:

    menuRecent = new QMenu(this);
    menuRecent->setIcon(QIcon::fromTheme("document-recent"));
    actionRecentClear = new QAction(QIcon::fromTheme("edit-delete"), QString(), this);
    connect(actionRecentClear, &QAction::triggered, app->settings, &Settings::clearRecentList);
    actionRecentNone = new QAction(this);
    actionRecentNone->setEnabled(false);
    connect(app->settings, &Settings::recentListUpdated, this, &MainWindow::updateRecentMenu);
    updateRecentMenu();

    // Tools Menu:

    auto actionKeyManager = app->actions.getOpenKeyManager(this);
    auto actionDeviceManager = app->actions.getOpenDeviceManager(this);
    auto actionAndroidExplorer = app->actions.getOpenAndroidExplorer(this);
    auto actionScreenshot = app->actions.getTakeScreenshot(this);
    actionProjectManager = new QAction(this);
    actionProjectManager->setIcon(QIcon::fromTheme("tool-projectmanager"));
    actionProjectManager->setShortcut(QKeySequence("Ctrl+M"));
    actionTitleEditor = new QAction(this);
    actionTitleEditor->setIcon(QIcon::fromTheme("tool-titleeditor"));
    actionTitleEditor->setShortcut(QKeySequence("Ctrl+T"));
    actionPermissionEditor = new QAction(this);
    actionPermissionEditor->setIcon(QIcon::fromTheme("tool-permissioneditor"));
    actionPermissionEditor->setShortcut(QKeySequence("Ctrl+Shift+P"));
    actionCloneApk = new QAction(this);
    actionCloneApk->setIcon(QIcon::fromTheme("edit-copy"));
    actionCloneApk->setShortcut(QKeySequence("Ctrl+Shift+R"));
    actionViewSignatures = new QAction(this);
    actionViewSignatures->setIcon(QIcon::fromTheme("view-certificate"));

    // Settings Menu:

    auto actionOptions = app->actions.getOpenOptions(this);

    // Window Menu:

    actionNewWindow = new QAction(this);
    actionNewWindow->setIcon(QIcon::fromTheme("window-new"));
    actionNewWindow->setShortcut(QKeySequence("Ctrl+N"));
    connect(actionNewWindow, &QAction::triggered, app, &Application::createNewInstance);

    // Help Menu:

    auto actionDonate = app->actions.getVisitDonatePage();
    actionAbout = new QAction(QIcon::fromTheme("apk-editor-studio"), QString(), this);
    actionAbout->setMenuRole(QAction::AboutRole);
    actionAboutQt = new QAction(QIcon::fromTheme("qt"), QString(), this);
    actionAboutQt->setMenuRole(QAction::AboutQtRole);

    // Menu Bar:

    menuFile = menuBar()->addMenu(QString());
    menuFile->addAction(actionApkOpen);
    menuFile->addMenu(menuRecent);
    menuFile->addSeparator();
    menuFile->addAction(actionApkSave);
    menuFile->addSeparator();
    menuFile->addAction(actionApkInstall);
    menuFile->addSeparator();
    menuFile->addAction(actionInstallExternal);
    menuFile->addAction(actionOptimizeExternal);
    menuFile->addAction(actionSignExternal);
    menuFile->addSeparator();
    menuFile->addAction(actionApkExplore);
    menuFile->addSeparator();
    menuFile->addAction(actionApkClose);
    menuFile->addSeparator();
    menuFile->addAction(app->actions.getExit(this));
    menuTab = menuBar()->addMenu(QString());
    menuTab->setEnabled(false);
    menuTools = menuBar()->addMenu(QString());
    menuTools->addAction(actionKeyManager);
    menuTools->addSeparator();
    menuTools->addAction(actionDeviceManager);
    menuTools->addAction(actionAndroidExplorer);
    menuTools->addAction(actionScreenshot);
    menuTools->addSeparator();
    menuTools->addAction(actionProjectManager);
    menuTools->addSeparator();
    menuTools->addAction(actionTitleEditor);
    menuTools->addAction(actionPermissionEditor);
    menuTools->addAction(actionCloneApk);
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
    menuHelp->addAction(app->actions.getCheckUpdates(this));
    menuHelp->addSeparator();
    menuHelp->addAction(actionAboutQt);
    menuHelp->addAction(actionAbout);

    // Tool Bar:

    actionFileSave = new QAction(QIcon::fromTheme("document-save"), QString(), this);
    actionFileSave->setEnabled(false);
    actionFileSaveAs = new QAction(QIcon::fromTheme("document-save-as"), QString(), this);
    actionFileSaveAs->setEnabled(false);

    toolbar = new Toolbar(this);
    toolbar->setObjectName("Toolbar");
    toolbar->addActionToPool("open-project", actionApkOpen);
    toolbar->addActionToPool("save-project", actionApkSave);
    toolbar->addActionToPool("install-project", actionApkInstall);
    toolbar->addActionToPool("open-contents", actionApkExplore);
    toolbar->addActionToPool("close-project", actionApkClose);
    toolbar->addActionToPool("save", actionFileSave);
    toolbar->addActionToPool("save-as", actionFileSaveAs);
    toolbar->addActionToPool("project-manager", actionProjectManager);
    toolbar->addActionToPool("title-editor", actionTitleEditor);
    toolbar->addActionToPool("permission-editor", actionPermissionEditor);
    toolbar->addActionToPool("rename-package", actionCloneApk);
    toolbar->addActionToPool("view-signatures", actionViewSignatures);
    toolbar->addActionToPool("device-manager", actionDeviceManager);
    toolbar->addActionToPool("android-explorer", actionAndroidExplorer);
    toolbar->addActionToPool("screenshot", actionScreenshot);
    toolbar->addActionToPool("key-manager", actionKeyManager);
    toolbar->addActionToPool("new-window", actionNewWindow);
    toolbar->addActionToPool("settings", actionOptions);
    toolbar->addActionToPool("donate", actionDonate);
    toolbar->initialize(app->settings->getMainWindowToolbar());
    addToolBar(toolbar);
    connect(toolbar, &Toolbar::updated, app->settings, &Settings::setMainWindowToolbar);

    // Signals / Slots:

    connect(actionApkSave, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->saveProject();
    });
    connect(actionApkInstall, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->installProject();
    });
    connect(actionApkExplore, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->exploreProject();
    });
    connect(actionApkClose, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->closeProject();
    });
    connect(actionTitleEditor, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->openTitlesTab();
    });
    connect(actionProjectManager, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->openProjectTab();
    });
    connect(actionPermissionEditor, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->openPermissionEditor();
    });
    connect(actionCloneApk, &QAction::triggered, this, [this]() {
        getCurrentProjectWidget()->openPackageRenamer();
    });
    connect(actionViewSignatures, &QAction::triggered, this, [this]() {
        const auto project = getCurrentProject();
        Q_ASSERT(project);
        SignatureViewer dialog(project->getOriginalPath(), this);
        dialog.exec();
    });
    connect(actionFileSave, &QAction::triggered, this, [this]() {
        qobject_cast<BaseEditableSheet *>(getCurrentTab())->save();
    });
    connect(actionFileSaveAs, &QAction::triggered, this, [this]() {
        qobject_cast<BaseFileSheet *>(getCurrentTab())->saveAs();
    });
    connect(actionAbout, &QAction::triggered, this, [this]() {
        AboutDialog about(this);
        about.exec();
    });
    connect(actionAboutQt, &QAction::triggered, app, &Application::aboutQt);
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
    //: Displayed as "Tab". Refers to a menu bar (along with File, Edit, View, Window, Help, and similar items).
    menuTab->setTitle(tr("Ta&b"));
    //: Refers to a menu bar (along with File, Edit, View, Window, Help, and similar items).
    menuTools->setTitle(tr("&Tools"));
    //: Refers to a menu bar (along with File, Edit, View, Window, Help, and similar items).
    menuSettings->setTitle(tr("&Settings"));
    //: Refers to a menu bar (along with File, Edit, View, Help, and similar items).
    menuWindow->setTitle(tr("&Window"));
    //: Refers to a menu bar (along with File, Edit, View, Window, and similar items).
    menuHelp->setTitle(tr("&Help"));

    // Tab Bar:

    actionFileSave->setText(tr("&Save"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionFileSaveAs->setText(tr("Save &As..."));

    // File Menu:

    actionApkSave->setText(tr("&Save APK..."));
    actionApkInstall->setText(tr("&Install APK..."));
    //: Displayed as "Open Contents".
    actionApkExplore->setText(tr("O&pen Contents"));
    actionApkClose->setText(tr("&Close APK"));

    // Recent Menu:

    menuRecent->setTitle(tr("Open &Recent"));
    actionRecentClear->setText(tr("&Clear List"));
    actionRecentNone->setText(tr("No Recent Files"));

    // Tools Menu:

    //: This string refers to a single project (as in "Manager of a project").
    actionProjectManager->setText(tr("&Project Manager"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionTitleEditor->setText(tr("Edit Application &Title"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionPermissionEditor->setText(tr("Edit Application &Permissions"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    tr("Edit Package &Name"); // For possible future use
    actionCloneApk->setText(tr("&Clone APK"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionViewSignatures->setText(tr("View &Signatures"));

    // Window Menu:

    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionNewWindow->setText(tr("Open &New Window"));
    menuWindow->clear();
    menuWindow->addAction(actionNewWindow);
    menuWindow->addSeparator();
    menuWindow->addActions(createPopupMenu()->actions());

    // Help Menu:

    //: Don't translate the "APK Editor Studio" part.
    actionAbout->setText(tr("&About APK Editor Studio..."));
    //: Don't translate the "&Qt" part.
    actionAboutQt->setText(tr("About &Qt..."));

    // Miscellaneous:

    toolbar->setWindowTitle(tr("Tools"));
    welcomeItemProxy->setData(welcomeItemProxy->index(0, 0), tr("Welcome"), Qt::DisplayRole);
}

void MainWindow::updateWindowForProject(Project *project)
{
    if (project) {
        setWindowTitle(QString("%1[*]").arg(project->getOriginalPath()));
        setWindowModified(project->getState().isModified());
        auto projectWidget = projectWidgets.value(project);
        updateWindowForTab(qobject_cast<BaseSheet *>(projectWidget->currentWidget()));
    } else {
        setWindowTitle(QString());
        setWindowModified(false);
        updateWindowForTab(nullptr);
    }

    actionApkSave->setEnabled(project ? project->getState().canSave() : false);
    actionApkInstall->setEnabled(project ? project->getState().canInstall() : false);
    actionApkExplore->setEnabled(project ? project->getState().canExplore() : false);
    actionApkClose->setEnabled(project ? project->getState().canClose() : false);
    actionTitleEditor->setEnabled(project ? project->getState().canEdit() : false);
    actionPermissionEditor->setEnabled(project ? project->getState().canEdit() : false);
    actionCloneApk->setEnabled(project ? project->getState().canEdit() : false);
    actionViewSignatures->setEnabled(project);
    actionProjectManager->setEnabled(project);
}

void MainWindow::updateWindowForTab(BaseSheet *tab)
{
    menuTab->clear();
    if (tab && !tab->actions().isEmpty()) {
        menuTab->setEnabled(true);
        menuTab->addActions(tab->actions());
    } else {
        menuTab->setEnabled(false);
    }
    actionFileSave->setEnabled(qobject_cast<const BaseEditableSheet *>(tab));
    actionFileSaveAs->setEnabled(qobject_cast<const BaseFileSheet *>(tab));
}

void MainWindow::updateRecentMenu()
{
    menuRecent->clear();
    auto recentList = app->settings->getRecentList();
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

void MainWindow::onProjectAdded(const QModelIndex &, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        const auto project = projects.at(i);
        auto projectWidget = new ProjectWidget(project, projects, this);
        projectWidgets.insert(project, projectWidget);
        connect(project, &Project::stateUpdated, this, [=]() {
            if (project == getCurrentProject()) {
                updateWindowForProject(project);
            }
        });
        connect(projectWidget, &ProjectWidget::currentTabChanged, this, [=](BaseSheet *tab) {
            if (project == getCurrentProject()) {
                updateWindowForTab(tab);
            }
        });
    }
}

void MainWindow::onProjectAboutToBeRemoved(const QModelIndex &, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        const auto project = projects.at(i);
        auto projectWidget = projectWidgets.take(project);
        projectWidget->deleteLater();
    }
}

void MainWindow::onProjectSwitched(Project *project)
{
    if (project) {
        centralWidget->set(projectWidgets.value(project));
    } else {
        centralWidget->set(welcomePage);
    }

    resourceTree->setModel(project ? &project->resourcesModel : dummyResourceModel);
    filesystemTree->setModel(project ? &project->filesystemModel : dummyFileSystemModel);
    iconList->setModel(project ? &project->iconsProxy : nullptr);
    logView->setModel(project ? &project->logModel : nullptr);
    manifestTable->setModel(project ? &project->manifestModel : nullptr);

    updateWindowForProject(project);
}

Project *MainWindow::addProject(const QString &path)
{
    if (auto existing = projects.existing(path)) {
        //: "%1" will be replaced with a path to an APK.
        const QString question = tr("This APK is already open:\n%1\nDo you want to reopen it and lose any unsaved changes?").arg(existing->getOriginalPath());
        const int answer = QMessageBox::question(this, QString(), question);
        if (answer != QMessageBox::Yes) {
            return nullptr;
        }
        projects.close(existing);
    }
    auto project = new Project(path);
    projects.add(project);
    setCurrentProject(project);
    return project;
}

Project *MainWindow::getCurrentProject() const
{
    const int row = welcomeItemProxy->mapToSourceRow(projectList->currentIndex());
    const auto index = projects.index(row);
    return index.isValid() ? static_cast<Project *>(index.internalPointer()) : nullptr;
}

ProjectWidget *MainWindow::getCurrentProjectWidget() const
{
    return projectWidgets.value(getCurrentProject());
}

BaseSheet *MainWindow::getCurrentTab() const
{
    if (auto projectWidget = getCurrentProjectWidget()) {
        return qobject_cast<BaseSheet *>(projectWidget->currentWidget());
    }
    return nullptr;
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
        QMapIterator<Project *, ProjectWidget *> it(projectWidgets);
        while (it.hasNext()) {
            it.next();
            auto tabs = it.value();
            if (tabs->isUnsaved()) {
                auto project = it.key();
                setCurrentProject(project);
                const QString question = tr("You have unsaved changes.\nDo you want to discard them and exit?");
                const int answer = QMessageBox::question(this, QString(), question, QMessageBox::Discard, QMessageBox::Cancel);
                if (answer != QMessageBox::Discard) {
                    event->ignore();
                    return;
                }
                break;
            }
        }
    }
    app->settings->setMainWindowGeometry(saveGeometry());
    app->settings->setMainWindowState(saveState());
    app->settings->setResourceTreeHeader(resourceTree->getView<ResourceTree *>()->header()->saveState());
    app->settings->setFileSystemTreeHeader(filesystemTree->getView<FileSystemTree *>()->header()->saveState());
    event->accept();
}
