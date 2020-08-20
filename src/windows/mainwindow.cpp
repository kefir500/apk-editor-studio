#include "windows/mainwindow.h"
#include "windows/aboutdialog.h"
#include "windows/dialogs.h"
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
#include "editors/fileeditor.h"
#include "editors/welcomeactionviewer.h"
#include "base/application.h"
#include "base/extralistitemproxy.h"
#include "base/updater.h"
#include "apk/project.h"
#include <QDebug>
#include <QDockWidget>
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

void MainWindow::setCurrentProject(Project *project)
{
    projectList->setCurrentProject(project);
}

void MainWindow::setInitialSize()
{
    resize(Utils::scale(1100, 600));
}

void MainWindow::initWidgets()
{
    qDebug() << "Initializing widgets...";

    setInitialSize();

    centralWidget = new CentralWidget(this);
    setCentralWidget(centralWidget);

    resourceTree = new ResourceAbstractView(new ResourceTree, this);
    connect(resourceTree, &ResourceAbstractView::editRequested, this, [this](const ResourceModelIndex &index) {
        getCurrentProjectWidget()->openResourceTab(index);
    });
    auto dockResourceWidget = new QWidget(this);
    auto resourceLayout = new QVBoxLayout(dockResourceWidget);
    resourceLayout->addWidget(resourceTree);
    resourceLayout->setMargin(0);

    filesystemTree = new ResourceAbstractView(new FileSystemTree, this);
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
    welcomeItemProxy->setData(welcomeItemProxy->index(0, 0), tr("Welcome"), Qt::DisplayRole);
    welcomeItemProxy->setData(welcomeItemProxy->index(0, 0), QIcon::fromTheme("apk-editor-studio"), Qt::DecorationRole);
    welcomePage = new WelcomeActionViewer(this);
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
    actionApkSave = new QAction(QIcon::fromTheme("apk-save"), QString(), this);
    actionApkSave->setShortcut(QKeySequence("Ctrl+Alt+S"));
    actionApkInstall = new QAction(QIcon::fromTheme("apk-install"), QString(), this);
    actionApkInstall->setShortcut(QKeySequence("Ctrl+I"));
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
    menuEditor->setEnabled(false);
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
    connect(actionFileSave, &QAction::triggered, this, [this]() {
        qobject_cast<Editor *>(getCurrentTab())->save();
    });
    connect(actionFileSaveAs, &QAction::triggered, this, [this]() {
        qobject_cast<FileEditor *>(getCurrentTab())->saveAs();
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

    // Tool Bar:

    toolbar->setWindowTitle(tr("Tools"));
    dockProjects->setWindowTitle(tr("Projects"));
    dockResources->setWindowTitle(tr("Resources"));
    dockFilesystem->setWindowTitle(tr("File System"));
    dockManifest->setWindowTitle(tr("Manifest"));
    dockIcons->setWindowTitle(tr("Icons"));
    actionFileSave->setText(tr("&Save"));
    actionFileSaveAs->setText(tr("Save &As..."));

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

    // Recent Menu:

    menuRecent->setTitle(tr("Open &Recent"));
    actionRecentClear->setText(tr("&Clear List"));
    actionRecentNone->setText(tr("No Recent Files"));

    // Tools Menu:

    //: This string refers to a single project (as in "Manager of a project").
    actionProjectManager->setText(tr("&Project Manager"));
    //: The "&" is a shortcut key, *not* a conjuction "and". Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionTitleEditor->setText(tr("Edit Application &Title"));
    //: The "&" is a shortcut key, *not* a conjuction "and". Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionPermissionEditor->setText(tr("Edit Application &Permissions"));

    // Window Menu:

    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionNewWindow->setText(tr("Open &New Window"));
    menuWindow->clear();
    menuWindow->addAction(actionNewWindow);
    menuWindow->addSeparator();
    menuWindow->addActions(createPopupMenu()->actions());

    // Help Menu:

    actionAbout->setText(tr("&About APK Editor Studio..."));
    actionAboutQt->setText(tr("About &Qt..."));
}

void MainWindow::updateWindowForProject(Project *project)
{
    if (project) {
        setWindowTitle(QString("%1[*]").arg(project->getOriginalPath()));
        setWindowModified(project->getState().isModified());
        auto projectWidget = projectWidgets.value(project);
        updateWindowForTab(qobject_cast<Viewer *>(projectWidget->currentWidget()));
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
    actionProjectManager->setEnabled(project);
}

void MainWindow::updateWindowForTab(Viewer *tab)
{
    menuEditor->clear();
    if (tab && !tab->actions().isEmpty()) {
        menuEditor->setEnabled(true);
        menuEditor->addActions(tab->actions());
    } else {
        menuEditor->setEnabled(false);
    }
    actionFileSave->setEnabled(qobject_cast<const Editor *>(tab));
    actionFileSaveAs->setEnabled(qobject_cast<const FileEditor *>(tab));
}

void MainWindow::updateRecentMenu()
{
    menuRecent->clear();
    auto recentList = app->settings->getRecentList();
    for (const RecentFile &recentEntry : recentList) {
        auto action = new QAction(recentEntry.thumbnail(), recentEntry.filename(), this);
        menuRecent->addAction(action);
        connect(action, &QAction::triggered, this, [=]() {
            app->actions.openApk(recentEntry.filename(), this);
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
        connect(projectWidget, &ProjectWidget::currentTabChanged, this, [=](Viewer *tab) {
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

    resourceTree->setModel(project ? &project->resourcesModel : nullptr);
    filesystemTree->setModel(project ? &project->filesystemModel : nullptr);
    iconList->setModel(project ? &project->iconsProxy : nullptr);
    logView->setModel(project ? &project->logModel : nullptr);
    manifestTable->setModel(project ? &project->manifestModel : nullptr);

    updateWindowForProject(project);
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

Viewer *MainWindow::getCurrentTab() const
{
    if (auto projectWidget = getCurrentProjectWidget()) {
        return qobject_cast<Viewer *>(projectWidget->currentWidget());
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
    if (instances == 1) {
        QMapIterator<Project *, ProjectWidget *> it(projectWidgets);
        while (it.hasNext()) {
            it.next();
            auto tabs = it.value();
            if (tabs->isUnsaved()) {
                auto project = it.key();
                projectList->setCurrentProject(project);
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
    event->accept();
}
