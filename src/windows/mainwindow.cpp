#include "windows/mainwindow.h"
#include "windows/aboutdialog.h"
#include "windows/dialogs.h"
#include "windows/permissioneditor.h"
#include "widgets/resourcetree.h"
#include "widgets/filesystemtree.h"
#include "widgets/iconlist.h"
#include "base/application.h"
#include "base/updater.h"
#include <QMenuBar>
#include <QDockWidget>
#include <QMimeData>
#include <QMimeDatabase>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setAcceptDrops(true);

    initWidgets();
    initMenus();
    loadSettings();

    connect(&app->projects, &ProjectItemsModel::changed, [=](Project *project) {
        if (project == projectsWidget->getCurrentProject()) {
            updateWindowForProject(project);
        }
    });

    connect(app->settings, &Settings::toolbarUpdated, [=]() {
        toolbar->reinitialize();
    });

    connect(app->settings, &Settings::resetDone, [=]() {
        restoreGeometry(QByteArray());
        setInitialSize();
        restoreState(defaultState);
    });

    QEvent languageChangeEvent(QEvent::LanguageChange);
    app->sendEvent(this, &languageChangeEvent);

    if (app->settings->getAutoUpdates()) {
        // Delay to prevent uninitialized window render
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [=]() {
            Updater::check(false, this);
            timer->deleteLater();
        });
        timer->setSingleShot(true);
        timer->start(1000);
    }

    qDebug();
}

void MainWindow::setInitialSize()
{
    resize(app->scale(1100, 600));
}

void MainWindow::initWidgets()
{
    qDebug() << "Initializing widgets...";
    setInitialSize();

    projectsWidget = new ProjectsWidget(this);
    projectsWidget->setModel(&app->projects);
    setCentralWidget(projectsWidget);
    connect(projectsWidget, &ProjectsWidget::currentProjectChanged, [=](Project *project) {
        setCurrentProject(project);
        projectList->setCurrentProject(project);
    });
    connect(projectsWidget, &ProjectsWidget::currentTabChanged, [=]() {
        menuEditor->clear();
        menuEditor->addActions(projectsWidget->getCurrentTabActions());
    });

    QWidget *dockProjectsWidget = new QWidget(this);
    QVBoxLayout *projectsLayout = new QVBoxLayout(dockProjectsWidget);
    projectList = new ProjectList(this);
    projectList->setModel(&app->projects);
    connect(projectList, &ProjectList::currentProjectChanged, [=](Project *project) {
        setCurrentProject(project);
        projectsWidget->setCurrentProject(project);
    });

    logView = new LogView(this);
    projectsLayout->addWidget(projectList);
    projectsLayout->addWidget(logView);
    projectsLayout->setMargin(0);
    projectsLayout->setSpacing(1);

    QWidget *dockResourceWidget = new QWidget(this);
    QVBoxLayout *resourceLayout = new QVBoxLayout(dockResourceWidget);
    resourceTree = new ResourceAbstractView(new ResourceTree, this);
    resourceLayout->addWidget(resourceTree);
    resourceLayout->setMargin(0);
    connect(resourceTree, &ResourceAbstractView::editRequested, projectsWidget, &ProjectsWidget::openResourceTab);

    QWidget *dockFilesystemWidget = new QWidget(this);
    QVBoxLayout *filesystemLayout = new QVBoxLayout(dockFilesystemWidget);
    filesystemTree = new ResourceAbstractView(new FileSystemTree, this);
    filesystemLayout->addWidget(filesystemTree);
    filesystemLayout->setMargin(0);
    connect(filesystemTree, &ResourceAbstractView::editRequested, projectsWidget, &ProjectsWidget::openResourceTab);

    QWidget *dockIconsWidget = new QWidget(this);
    QVBoxLayout *iconsLayout = new QVBoxLayout(dockIconsWidget);
    iconList = new ResourceAbstractView(new IconList, this);
    iconsLayout->addWidget(iconList);
    iconsLayout->setMargin(0);
    iconsLayout->setSpacing(1);
    connect(iconList, &ResourceAbstractView::editRequested, projectsWidget, &ProjectsWidget::openResourceTab);

    QWidget *dockManifestWidget = new QWidget(this);
    QVBoxLayout *manifestLayout = new QVBoxLayout(dockManifestWidget);
    manifestTable = new ManifestView(this);
    manifestLayout->addWidget(manifestTable);
    manifestLayout->setMargin(0);
    connect(manifestTable, &ManifestView::titleEditorRequested, projectsWidget, &ProjectsWidget::openTitlesTab);

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
}

void MainWindow::initMenus()
{
    qDebug() << "Initializing menus...";

    // File Menu:

    auto actionApkOpen = app->actions.getOpenApk(this);
    actionApkSave = new QAction(app->icons.get("pack.png"), QString(), this);
    actionApkSave->setShortcut(QKeySequence("Ctrl+Alt+S"));
    actionApkInstall = new QAction(app->icons.get("install.png"), QString(), this);
    actionApkInstall->setShortcut(QKeySequence("Ctrl+I"));
    actionApkExplore = new QAction(app->icons.get("explore.png"), QString(), this);
    actionApkExplore->setShortcut(QKeySequence("Ctrl+E"));
    actionApkClose = new QAction(app->icons.get("x-blue.png"), QString(), this);
    actionApkClose->setShortcut(QKeySequence("Ctrl+W"));

    // Tools Menu:

    auto actionKeyManager = app->actions.getOpenKeyManager(this);
    auto actionDeviceManager = app->actions.getOpenDeviceManager(this);
    auto actionAndroidExplorer = app->actions.getOpenAndroidExplorer(this);
    auto actionScreenshot = app->actions.getTakeScreenshot(this);
    actionProjectManager = new QAction(this);
    actionProjectManager->setIcon(app->icons.get("project.png"));
    actionProjectManager->setShortcut(QKeySequence("Ctrl+M"));
    actionTitleEditor = new QAction(this);
    actionTitleEditor->setIcon(app->icons.get("title.png"));
    actionTitleEditor->setShortcut(QKeySequence("Ctrl+T"));
    actionPermissionEditor = new QAction(this);
    actionPermissionEditor->setIcon(app->icons.get("permissions.png"));
    actionPermissionEditor->setShortcut(QKeySequence("Ctrl+Shift+P"));

    // Settings Menu:

    auto actionOptions = app->actions.getOpenOptions(this);

    // Help Menu:

    auto actionDonate = app->actions.getVisitDonatePage();
    actionAbout = new QAction(app->icons.get("application.png"), QString(), this);
    actionAbout->setMenuRole(QAction::AboutRole);
    actionAboutQt = new QAction(app->icons.get("qt.png"), QString(), this);
    actionAboutQt->setMenuRole(QAction::AboutQtRole);

    // Menu Bar:

    menuFile = menuBar()->addMenu(QString());
    menuFile->addAction(actionApkOpen);
    menuFile->addMenu(app->actions.getRecent(this));
    menuFile->addSeparator();
    menuFile->addAction(actionApkSave);
    menuFile->addSeparator();
    menuFile->addAction(actionApkInstall);
    menuFile->addSeparator();
    menuFile->addAction(app->actions.getInstallApk(this));
    menuFile->addAction(app->actions.getOptimizeApk(this));
    menuFile->addAction(app->actions.getSignApk(this));
    menuFile->addSeparator();
    menuFile->addAction(actionApkExplore);
    menuFile->addSeparator();
    menuFile->addAction(actionApkClose);
    menuFile->addSeparator();
    menuFile->addAction(app->actions.getExit(this));
    menuEditor = menuBar()->addMenu(QString());
    menuEditor->addActions(projectsWidget->getCurrentTabActions());
    menuTools = menuBar()->addMenu(QString());
    menuTools->addAction(actionKeyManager);
    menuTools->addSeparator();
    menuTools->addAction(actionDeviceManager);
    menuTools->addAction(actionAndroidExplorer);
    menuTools->addAction(actionScreenshot);
    menuTools->addSeparator();
    menuTools->addAction(actionProjectManager);
    menuTools->addAction(actionTitleEditor);
    menuTools->addAction(actionPermissionEditor);
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

    Toolbar::addToPool("open-project", actionApkOpen);
    Toolbar::addToPool("save-project", actionApkSave);
    Toolbar::addToPool("install-project", actionApkInstall);
    Toolbar::addToPool("open-contents", actionApkExplore);
    Toolbar::addToPool("close-project", actionApkClose);
    Toolbar::addToPool("project-manager", actionProjectManager);
    Toolbar::addToPool("title-editor", actionTitleEditor);
    Toolbar::addToPool("permission-editor", actionPermissionEditor);
    Toolbar::addToPool("device-manager", actionDeviceManager);
    Toolbar::addToPool("android-explorer", actionAndroidExplorer);
    Toolbar::addToPool("screenshot", actionScreenshot);
    Toolbar::addToPool("key-manager", actionKeyManager);
    Toolbar::addToPool("settings", actionOptions);
    Toolbar::addToPool("donate", actionDonate);
    toolbar = new Toolbar(this);
    toolbar->setObjectName("Toolbar");
    addToolBar(toolbar);

    setActionsEnabled(nullptr);

    // Signals / Slots

    connect(actionApkSave, &QAction::triggered, projectsWidget, &ProjectsWidget::saveCurrentProject);
    connect(actionApkInstall, &QAction::triggered, projectsWidget, &ProjectsWidget::installCurrentProject);
    connect(actionApkExplore, &QAction::triggered, projectsWidget, &ProjectsWidget::exploreCurrentProject);
    connect(actionApkClose, &QAction::triggered, projectsWidget, &ProjectsWidget::closeCurrentProject);
    connect(actionTitleEditor, &QAction::triggered, projectsWidget, &ProjectsWidget::openTitlesTab);
    connect(actionProjectManager, &QAction::triggered, projectsWidget, &ProjectsWidget::openProjectTab);
    connect(actionPermissionEditor, &QAction::triggered, [=]() {
        const auto project = projectsWidget->getCurrentProject();
        PermissionEditor permissionEditor(project->manifest, this);
        permissionEditor.exec();
    });
    connect(actionAboutQt, &QAction::triggered, app, &Application::aboutQt);
    connect(actionAbout, &QAction::triggered, [=]() {
        AboutDialog about(this);
        about.exec();
    });
}

void MainWindow::retranslate()
{
    tr("Remove Temporary Files..."); // TODO For future use

    // Tool Bar:

    toolbar->setWindowTitle(tr("Tools"));
    dockProjects->setWindowTitle(tr("Projects"));
    dockResources->setWindowTitle(tr("Resources"));
    dockFilesystem->setWindowTitle(tr("File System"));
    dockManifest->setWindowTitle(tr("Manifest"));
    dockIcons->setWindowTitle(tr("Icons"));

    // Menu Bar:

    menuFile->setTitle(tr("&File"));
    menuEditor->setTitle(tr("&Editor"));
    menuTools->setTitle(tr("&Tools"));
    menuSettings->setTitle(tr("&Settings"));
    menuWindow->setTitle(tr("&Window"));
    menuHelp->setTitle(tr("&Help"));

    // File Menu:

    actionApkSave->setText(tr("&Save APK..."));
    actionApkInstall->setText(tr("&Install APK..."));
    actionApkExplore->setText(tr("O&pen Contents"));
    actionApkClose->setText(tr("&Close APK"));

    // Tools Menu:

    //: This string refers to a single project (as in "Manager of a project").
    actionProjectManager->setText(tr("&Project Manager"));
    //: The "&" is a shortcut key, *not* a conjuction "and". Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionTitleEditor->setText(tr("Edit Application &Title"));
    //: The "&" is a shortcut key, *not* a conjuction "and". Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionPermissionEditor->setText(tr("Edit Application &Permissions"));

    // Window Menu:

    menuWindow->clear();
    menuWindow->addActions(createPopupMenu()->actions());

    // Help Menu:

    actionAbout->setText(tr("&About APK Editor Studio..."));
    actionAboutQt->setText(tr("About &Qt..."));
}

void MainWindow::loadSettings()
{
    qDebug() << "Loading settings...";
    restoreGeometry(app->settings->getMainWindowGeometry());
    restoreState(app->settings->getMainWindowState());
}

void MainWindow::saveSettings()
{
    app->settings->setMainWindowGeometry(saveGeometry());
    app->settings->setMainWindowState(saveState());
}

bool MainWindow::setCurrentProject(Project *project)
{
    updateWindowForProject(project);
    resourceTree->setModel(project ? &project->resourcesModel : nullptr);
    filesystemTree->setModel(project ? &project->filesystemModel : nullptr);
    iconList->setModel(project ? &project->iconsProxy : nullptr);
    logView->setModel(project ? &project->logModel : nullptr);
    manifestTable->setModel(project ? &project->manifestModel : nullptr);
    return project;
}

void MainWindow::setActionsEnabled(const Project *project)
{
    actionApkSave->setEnabled(project ? project->getState().canSave() : false);
    actionApkInstall->setEnabled(project ? project->getState().canInstall() : false);
    actionApkExplore->setEnabled(project ? project->getState().canExplore() : false);
    actionApkClose->setEnabled(project ? project->getState().canClose() : false);
    actionTitleEditor->setEnabled(project ? project->getState().canEdit() : false);
    actionPermissionEditor->setEnabled(project ? project->getState().canEdit() : false);
    actionProjectManager->setEnabled(project);
}

void MainWindow::updateWindowForProject(const Project *project)
{
    if (project) {
        setWindowTitle(QString("%1[*]").arg(project->getOriginalPath()));
        setWindowModified(project->getState().isModified());
        setActionsEnabled(project);
    } else {
        setWindowTitle(QString());
        setWindowModified(false);
        setActionsEnabled(nullptr);
    }
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
        const bool showRubberBand = mimeData->urls().first().toString().toLower().endsWith(".apk");
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
        QList<QUrl> urls = mimeData->urls();
        for (const QUrl &url : urls) {
            const QString file = url.toLocalFile();
            const QMimeType mime = QMimeDatabase().mimeTypeForFile(file);
            if (mime.inherits("application/zip")) {
                app->actions.openApk(file, this);
                event->acceptProposedAction();
            }
        }
    }
    rubberBand->hide();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const bool unsaved = projectsWidget->hasUnsavedProjects();
    if (unsaved) {
        const QString question = tr("You have unsaved changes.\nDo you want to discard them and exit?");
        const int answer = QMessageBox::question(this, QString(), question, QMessageBox::Discard, QMessageBox::Cancel);
        if (answer != QMessageBox::Discard) {
            event->ignore();
            return;
        }
    }
    saveSettings();
    event->accept();
}
