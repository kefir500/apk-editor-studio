#include "windows/mainwindow.h"
#include "windows/aboutdialog.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "windows/keymanager.h"
#include "windows/settingsdialog.h"
#include "windows/waitdialog.h"
#include "widgets/decorationdelegate.h"
#include "widgets/logdelegate.h"
#include "widgets/projectdelegate.h"
#include "widgets/resourceviewcontainer.h"
#include "base/application.h"
#include "base/debug.h"
#include <QBoxLayout>
#include <QHeaderView>
#include <QMenuBar>
#include <QDockWidget>
#include <QFileInfo>
#include <QDirIterator>
#include <QMimeData>
#include <QMimeDatabase>

// TODO Add SVG support

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setAcceptDrops(true);

    initWidgets();
    initMenus();
    loadSettings();
    initLanguages();

    connect(app->recent, &Recent::changed, this, &MainWindow::updateRecentMenu);
    updateRecentMenu();

    QEvent languageChangeEvent(QEvent::LanguageChange);
    app->sendEvent(this, &languageChangeEvent);

    if (app->settings->getAutoUpdates()) {
        Updater *updater = new Updater(false, this);
        updater->checkAndDelete();
    }
}

void MainWindow::setInitialSize()
{
    resize(app->dpiAwareSize(1200, 650));
}

void MainWindow::initWidgets()
{
    setInitialSize();

    tabs = new QStackedWidget(this);
    setCentralWidget(tabs);

    welcome = new WelcomeTab(this);
    tabs->addWidget(welcome);

    QWidget *dockProjectsWidget = new QWidget(this);
    QVBoxLayout *projectsLayout = new QVBoxLayout(dockProjectsWidget);
    projectsList = new QComboBox(this);
    projectsList->setModel(&app->projects);
    projectsList->setMinimumHeight(38);
    projectsList->setIconSize(QSize(32, 32));
    projectsList->setItemDelegate(new ProjectDelegate(this));
    logView = new LogView(this);
    logView->setItemDelegate(new LogDelegate(this));
    projectsLayout->addWidget(projectsList);
    projectsLayout->addWidget(logView);
    projectsLayout->setMargin(0);

    QWidget *dockResourceWidget = new QWidget(this);
    QVBoxLayout *resourceLayout = new QVBoxLayout(dockResourceWidget);
    resourcesTree = new QTreeView(this);
    resourcesTree->setContextMenuPolicy(Qt::CustomContextMenu);
    resourcesTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    resourcesTree->setItemDelegate(new DecorationDelegate(QSize(16, 16), this));
    ResourceViewContainer *resourceTreeContainer = new ResourceViewContainer(resourcesTree, this);
    resourceLayout->addWidget(resourceTreeContainer);
    resourceLayout->setMargin(0);
    connect(resourceTreeContainer, &ResourceViewContainer::requestForEdit, this, &MainWindow::openResourceItem);

    QWidget *dockFilesystemWidget = new QWidget(this);
    QVBoxLayout *filesystemLayout = new QVBoxLayout(dockFilesystemWidget);
    filesystemTree = new QTreeView(this);
    filesystemTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ResourceViewContainer *filesystemTreeContainer = new ResourceViewContainer(filesystemTree, this);
    filesystemLayout->addWidget(filesystemTreeContainer);
    filesystemLayout->setMargin(0);
    connect(filesystemTreeContainer, &ResourceViewContainer::requestForEdit, this, &MainWindow::openFilesystemItem);

    QWidget *dockIconsWidget = new QWidget(this);
    QVBoxLayout *iconsLayout = new QVBoxLayout(dockIconsWidget);
    iconsList = new IconList(this);
    iconsList->setIconSize(QSize(32, 32));
    iconsList->setContextMenuPolicy(Qt::CustomContextMenu);
    ResourceViewContainer *iconsListContainer = new ResourceViewContainer(iconsList, this);
    iconsLayout->addWidget(iconsListContainer);
    iconsLayout->setMargin(0);
    connect(iconsListContainer, &ResourceViewContainer::requestForEdit, this, &MainWindow::openIconItem);

    QWidget *dockManifestWidget = new QWidget(this);
    QVBoxLayout *manifestLayout = new QVBoxLayout(dockManifestWidget);
    manifestTable = new ManifestView(this);
    manifestLayout->addWidget(manifestTable);
    manifestLayout->setMargin(0);
    connect(manifestTable, &ManifestView::editorRequested, this, &MainWindow::openManifestEditor);

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

    defaultState = saveState();

    connect(projectsList, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, static_cast<void (MainWindow::*)(int)>(&MainWindow::setCurrentProject));
    connect(logView, &QListView::clicked, this, &MainWindow::openLogEntry);
    connect(logView, &QListView::clicked, logView, &QListView::clearSelection);
    connect(&app->projects, &ProjectsModel::added, this, &MainWindow::onProjectAdded);
    connect(&app->projects, &ProjectsModel::removed, this, &MainWindow::onProjectRemoved);
}

void MainWindow::initMenus()
{
    qDebug() << "Initializing menus...";

    // File Menu:

    actionApkOpen = new QAction(app->loadIcon("open.png"), QString(), this);
    actionApkOpen->setShortcut(QKeySequence::Open);
    actionApkExplore = new QAction(app->loadIcon("explore.png"), QString(), this);
    actionApkExplore->setShortcut(QKeySequence("Ctrl+E"));
    actionResourceSave = new QAction(app->loadIcon("save.png"), QString(), this);
    actionResourceSave->setShortcut(QKeySequence::Save);
    actionResourceSaveAs = new QAction(app->loadIcon("save-as.png"), QString(), this);
    actionApkSave = new QAction(app->loadIcon("pack.png"), QString(), this);
    actionApkSave->setShortcut(QKeySequence("Ctrl+Shift+S"));
    actionApkInstall = new QAction(app->loadIcon("device.png"), QString(), this);
    actionApkInstall->setShortcut(QKeySequence("Ctrl+I"));
    actionApkClose = new QAction(app->loadIcon("close-project.png"), QString(), this);
    actionApkClose->setShortcut(QKeySequence("Ctrl+W"));
    actionExit = new QAction(QIcon(app->loadIcon("close.png")), QString(), this);
    actionExit->setShortcut(QKeySequence::Quit);

    // Recent Menu:

    menuRecent = new QMenu(this);
    menuRecent->setIcon(app->loadIcon("recent.png"));
    actionRecentClear = new QAction(app->loadIcon("close.png"), QString(), this);
    actionRecentNone = new QAction(this);
    actionRecentNone->setEnabled(false);

    // Tools Menu:

    actionKeyManager = new QAction(this);
    actionKeyManager->setIcon(app->loadIcon("key.png"));
    actionKeyManager->setShortcut(QKeySequence("Ctrl+K"));
    actionDeviceManager = new QAction(this);
    actionDeviceManager->setIcon(app->loadIcon("device.png"));
    actionDeviceManager->setShortcut(QKeySequence("Ctrl+D"));
    actionProjectManager = new QAction(this);
    actionProjectManager->setIcon(app->loadIcon("project.png"));
    actionProjectManager->setShortcut(QKeySequence("Ctrl+R"));
    actionTitleEditor = new QAction(this);
    actionTitleEditor->setIcon(app->loadIcon("title.png"));
    actionTitleEditor->setShortcut(QKeySequence("Ctrl+T"));

    // Settings Menu:

    actionOptions = new QAction(this);
    actionOptions->setIcon(app->loadIcon("settings.png"));
    actionOptions->setShortcut(QKeySequence("Ctrl+P"));
    menuLanguage = new QMenu(this);
    actionAssociate = new QAction(this);
    actionAssociate->setIcon(app->loadIcon("application.png"));
    actionSettingsReset = new QAction(this);
    actionSettingsReset->setIcon(app->loadIcon("close.png"));

    // Help Menu:

    actionWebsite = new QAction(app->loadIcon("website.png"), QString(), this);
    actionUpdate = new QAction(app->loadIcon("update.png"), QString(), this);
    actionDonate = new QAction(app->loadIcon("donate.png"), QString(), this);
    actionAbout = new QAction(app->loadIcon("application.png"), QString(), this);
    actionAboutQt = new QAction(app->loadIcon("qt.png"), QString(), this);

    // Tool Bar:

    toolbar = new Toolbar(this);
    toolbar->setObjectName("Toolbar");
    app->toolbar.insert("open-project", actionApkOpen);
    app->toolbar.insert("save-project", actionApkSave);
    app->toolbar.insert("close-project", actionApkClose);
    app->toolbar.insert("save-resource", actionResourceSave);
    app->toolbar.insert("save-resource-as", actionResourceSaveAs);
    app->toolbar.insert("install-project", actionApkInstall);
    app->toolbar.insert("open-contents", actionApkExplore);
    app->toolbar.insert("project-manager", actionProjectManager);
    app->toolbar.insert("title-editor", actionTitleEditor);
    app->toolbar.insert("device-manager", actionDeviceManager);
    app->toolbar.insert("key-manager", actionKeyManager);
    app->toolbar.insert("settings", actionOptions);
    app->toolbar.insert("donate", actionDonate);
    addToolBar(toolbar);

    // Menu Bar:

    menuFile = menuBar()->addMenu(QString());
    menuFile->addAction(actionApkOpen);
    menuFile->addMenu(menuRecent);
    menuFile->addSeparator();
    menuFile->addAction(actionApkExplore);
    menuFile->addSeparator();
    menuFile->addAction(actionResourceSave);
    menuFile->addAction(actionResourceSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(actionApkSave);
    menuFile->addAction(actionApkInstall);
    menuFile->addSeparator();
    menuFile->addAction(actionApkClose);
    menuFile->addSeparator();
    menuFile->addAction(actionExit);
    menuTools = menuBar()->addMenu(QString());
    menuTools->addAction(actionKeyManager);
    menuTools->addAction(actionDeviceManager);
    menuTools->addSeparator();
    menuTools->addAction(actionProjectManager);
    menuTools->addAction(actionTitleEditor);
    menuSettings = menuBar()->addMenu(QString());
    menuSettings->addAction(actionOptions);
    menuSettings->addSeparator();
    menuSettings->addMenu(menuLanguage);
    menuSettings->addSeparator();
    menuSettings->addAction(actionAssociate);
    menuSettings->addSeparator();
    menuSettings->addAction(actionSettingsReset);
    menuWindow = menuBar()->addMenu(QString());
    menuHelp = menuBar()->addMenu(QString());
    menuHelp->addAction(actionWebsite);
    menuHelp->addAction(actionUpdate);
    menuHelp->addAction(actionDonate);
    menuHelp->addSeparator();
    menuHelp->addAction(actionAboutQt);
    menuHelp->addAction(actionAbout);

    setActionsEnabled(nullptr);

    // Signals / Slots

    connect(actionApkOpen, &QAction::triggered, [=]() { getCurrentProjectWidget()->openProject(); });
    connect(actionApkSave, &QAction::triggered, [=]() { getCurrentProjectWidget()->saveProject(); });
    connect(actionApkInstall, &QAction::triggered, [=]() {
        const bool isProjectOpen = projectWidgets.count();
        isProjectOpen ? getCurrentProjectWidget()->installProject() : app->installExternalApk();
    });
    connect(actionApkClose, &QAction::triggered, [=]() { getCurrentProjectWidget()->closeProject(); });
    connect(actionApkExplore, &QAction::triggered, [=]() { getCurrentProjectWidget()->exploreProject(); });
    connect(actionProjectManager, &QAction::triggered, [=]() { getCurrentProjectWidget()->openProjectTab(); });
    connect(actionTitleEditor, &QAction::triggered, [=]() { getCurrentProjectWidget()->openTitlesTab(); });
    connect(actionResourceSave, &QAction::triggered, [=]() { getCurrentProjectWidget()->saveTab(); });
    connect(actionResourceSaveAs, &QAction::triggered, [=]() { getCurrentProjectWidget()->saveTabAs(); });
    connect(actionRecentClear, &QAction::triggered, app->recent, &Recent::clear);
    connect(actionOptions, &QAction::triggered, [=]() {
        SettingsDialog settings(this);
        settings.exec();
        toolbar->reinitialize();
    });
    connect(actionKeyManager, &QAction::triggered, [=]() {
        KeyManager keyManager(this);
        keyManager.exec();
    });
    connect(actionDeviceManager, &QAction::triggered, [=]() {
        DeviceManager deviceManager(this);
        deviceManager.exec();
    });
    connect(actionAssociate, &QAction::triggered, app, &Application::associate);
    connect(actionSettingsReset, &QAction::triggered, this, &MainWindow::resetSettings);
    connect(actionWebsite, &QAction::triggered, app, &Application::visitWebPage);
    connect(actionUpdate, &QAction::triggered, [=]() {
        Updater *updater = new Updater(true, this);
        updater->checkAndDelete();
    });
    connect(actionDonate, &QAction::triggered, app, &Application::visitDonatePage);
    connect(actionAboutQt, &QAction::triggered, app, &Application::aboutQt);
    connect(actionAbout, &QAction::triggered, [=]() {
        AboutDialog about(this);
        about.exec();
    });
}

void MainWindow::initLanguages()
{
    QList<Language> languages = app->getLanguages();

    actionsLanguage = new QActionGroup(this);
    actionsLanguage->setExclusive(true);
    foreach (const Language &language, languages) {
        const QString localeTitle = language.getTitle();
        const QString localeCode = language.getCode();
        const QPixmap localeFlag = language.getFlag();
        QAction *action = actionsLanguage->addAction(localeFlag, localeTitle);
        action->setCheckable(true);
        action->setProperty("locale", localeCode);
        connect(action, &QAction::triggered, [=]() {
            app->setLanguage(localeCode);
        });
    }
    menuLanguage->addActions(actionsLanguage->actions());
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
    menuTools->setTitle(tr("&Tools"));
    menuSettings->setTitle(tr("&Settings"));
    menuWindow->setTitle(tr("&Window"));
    menuHelp->setTitle(tr("&Help"));

    // File Menu:

    actionApkOpen->setText(tr("&Open APK..."));
    actionApkExplore->setText(tr("Open Con&tents"));
    actionResourceSave->setText(tr("&Save Resource"));
    actionResourceSaveAs->setText(tr("Save Resource &As..."));
    actionApkSave->setText(tr("Save A&PK..."));
    actionApkClose->setText(tr("&Close APK..."));
    actionApkInstall->setText(tr("&Install APK..."));
    actionExit->setText(tr("E&xit"));

    // Recent Menu:

    menuRecent->setTitle(tr("&Recent Files"));
    actionRecentClear->setText(tr("&Clear List"));
    actionRecentNone->setText(tr("No Recent Files"));

    // Tools Menu:

    actionKeyManager->setText(tr("&Key Manager..."));
    actionDeviceManager->setText(tr("&Device Manager..."));
    actionProjectManager->setText(tr("&Project Manager"));
    actionTitleEditor->setText(tr("Edit Application &Title"));

    // Settings Menu:

    actionOptions->setText(tr("&Options..."));
    menuLanguage->setTitle(tr("&Language"));
    actionSettingsReset->setText(tr("&Reset Settings..."));
    actionAssociate->setText(tr("Set as Default &Program"));

    // Window Menu:

    menuWindow->clear();
    menuWindow->addActions(createPopupMenu()->actions());

    // Help Menu:

    actionWebsite->setText(tr("Visit &Website"));
    actionUpdate->setText(tr("Check for &Updates"));
    actionDonate->setText(tr("Make a &Donation"));
    actionAbout->setText(tr("&About APK Editor Studio..."));
    actionAboutQt->setText(tr("About &Qt..."));
}

void MainWindow::loadSettings()
{
    qDebug() << "Loading settings...";
    restoreGeometry(app->settings->getMainWindowGeometry());
    restoreState(app->settings->getMainWindowState());
    toolbar->reinitialize();
}

void MainWindow::resetSettings()
{
    if (app->settings->reset(this)) {
        restoreGeometry(QByteArray());
        restoreState(defaultState);
        toolbar->reinitialize();
        restoreState(defaultState);
        setInitialSize();
    }
}

void MainWindow::saveSettings()
{
    app->settings->setMainWindowGeometry(saveGeometry());
    app->settings->setMainWindowState(saveState());
}

BaseEditor *MainWindow::openResourceItem(const QModelIndex &index)
{
    const ResourcesModel *model = static_cast<ResourcesModel *>(resourcesTree->model());
    if (model->hasChildren(index)) {
        return nullptr;
    }
    const QString path = model->getResourcePath(index);
    const QPixmap icon = model->getResourceFlag(index);
    return getCurrentProjectWidget()->openResourceTab(path, icon);
}

BaseEditor *MainWindow::openIconItem(const QModelIndex &index)
{
    const IconsProxy *proxy = static_cast<IconsProxy *>(iconsList->model());
    const QModelIndex sourceIndex = proxy->mapToSource(index);
    return openResourceItem(sourceIndex);
}

BaseEditor *MainWindow::openFilesystemItem(const QModelIndex &index)
{
    const QFileSystemModel *model = static_cast<QFileSystemModel *>(filesystemTree->model());
    if (model->hasChildren(index)) {
        return nullptr;
    }
    const QString path = model->filePath(index);
    return getCurrentProjectWidget()->openResourceTab(path);
}

BaseEditor *MainWindow::openManifestEditor(ManifestModel::ManifestRow manifestField)
{
    if (manifestField == ManifestModel::ApplicationLabel) {
        return getCurrentProjectWidget()->openTitlesTab();
    }
    return nullptr;
}

void MainWindow::openLogEntry(const QModelIndex &index)
{
    const QString message = index.sibling(index.row(), LogModel::LogDescriptive).data().toString();
    if (!message.isEmpty()) {
        Dialogs::log(message, this);
    }
}

void MainWindow::onProjectAdded(Project *project)
{
    ProjectWidget *projectWidget = new ProjectWidget(project, this);
    projectWidgets.insert(project, projectWidget);
    tabs->addWidget(projectWidget);

    projectsList->setCurrentIndex(app->projects.indexOf(project));
    connect(project, &Project::dataChanged, [=]() {
        if (project == getCurrentProject()) {
            hello(project);
        }
    });
}

void MainWindow::onProjectRemoved(Project *project)
{
    delete projectWidgets.value(project);
    projectWidgets.remove(project);
}

void MainWindow::setCurrentProject(Project *project)
{
    hello(project);

    if (!project) {
        tabs->setCurrentWidget(welcome);
        return;
    }

    ProjectWidget *projectWidget = projectWidgets.value(project, nullptr);
    tabs->setCurrentWidget(projectWidget);

    resourcesTree->setModel(&project->resourcesModel);
    resourcesTree->sortByColumn(0, Qt::DescendingOrder);
    resourcesTree->setColumnWidth(ResourcesModel::NodeCaption, 120);
    resourcesTree->setColumnWidth(ResourcesModel::ResourceLocale, 64);
    resourcesTree->setColumnWidth(ResourcesModel::ResourcePath, 500);
    filesystemTree->setModel(&project->filesystemModel);
    filesystemTree->setRootIndex(project->filesystemModel.index(project->getContentsPath()));
    iconsList->setModel(&project->iconsProxy);
    logView->setModel(&project->logModel);
    manifestTable->setModel(&project->manifestModel);
}

void MainWindow::setCurrentProject(int index)
{
    Project *project = nullptr;
    const QModelIndex modelIndex = projectsList->model()->index(index, 0);
    if (modelIndex.isValid()) {
        project = static_cast<Project *>(modelIndex.internalPointer());
    }
    return setCurrentProject(project);
}

void MainWindow::hello(const Project *project)
{
    if (project) {
        setWindowTitle(QString("%1 [*]").arg(project->getOriginalPath()));
        setWindowModified(project->getModifiedState());
        setActionsEnabled(project);
    } else {
        setWindowTitle(QString());
        setWindowModified(false);
        setActionsEnabled(nullptr);
    }
}

void MainWindow::setActionsEnabled(const Project *project)
{
    actionApkSave->setEnabled(false);
    actionApkClose->setEnabled(false);
    actionApkInstall->setEnabled(false);
    actionApkExplore->setEnabled(false);
    actionResourceSave->setEnabled(false);
    actionResourceSaveAs->setEnabled(false);
    actionProjectManager->setEnabled(false);
    actionTitleEditor->setEnabled(false);

    if (!project) {
        actionApkInstall->setEnabled(true);
        return;
    }

    switch (project->getState()) {
    case Project::ProjectReady:
        actionApkClose->setEnabled(true);
    case Project::ProjectInstalling:
        actionApkSave->setEnabled(true);
        actionApkInstall->setEnabled(true);
        actionTitleEditor->setEnabled(true);
    case Project::ProjectPacking:
    case Project::ProjectSigning:
    case Project::ProjectOptimizing:
        actionResourceSave->setEnabled(true);
        actionResourceSaveAs->setEnabled(true);
    case Project::ProjectUnpacking:
        actionApkExplore->setEnabled(true);
    case Project::ProjectNone:
        actionProjectManager->setEnabled(true);
        if (project->getErroredState()) {
            actionApkClose->setEnabled(true);
        }
    }
}

void MainWindow::updateRecentMenu()
{
    menuRecent->clear();
    QList<RecentFile> recentList = app->recent->all();
    foreach (const RecentFile &recentEntry, recentList) {
        QAction *action = new QAction(recentEntry.thumbnail, recentEntry.filename, this);
        menuRecent->addAction(action);
        connect(action, &QAction::triggered, [&]() {
            app->openApk(recentEntry.filename);
        });
    }
    menuRecent->addSeparator();
    menuRecent->addAction(recentList.isEmpty() ? actionRecentNone : actionRecentClear);
}

Project *MainWindow::getCurrentProject() const
{
    const QModelIndex index = projectsList->model()->index(projectsList->currentIndex(), 0);
    return static_cast<Project *>(index.internalPointer());
}

ProjectWidget *MainWindow::getCurrentProjectWidget() const
{
    return static_cast<ProjectWidget *>(tabs->currentWidget());
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
        const QString currentLocale = app->settings->getLanguage();
        QIcon flag(app->getLocaleFlag(currentLocale));
        menuLanguage->setIcon(flag);
        foreach (QAction *action, actionsLanguage->actions()) {
            if (action->property("locale") == currentLocale) {
                action->setChecked(true);
                break;
            }
        }
    } else {
        QWidget::changeEvent(event);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        foreach (const QUrl &url, urls) {
            const QString file = url.toLocalFile();
            const QMimeType mime = QMimeDatabase().mimeTypeForFile(file);
            if (mime.inherits("application/zip")) {
                app->openApk(file);
                event->acceptProposedAction();
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool unsaved = false;
    QList<ProjectWidget *> tabs = projectWidgets.values();
    foreach (ProjectWidget *tab, tabs) {
        if (tab->hasUnsavedProject()) {
            setCurrentProject(tab->getProject());
            unsaved = true;
            break;
        }
    }
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
