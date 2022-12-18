#include "widgets/projectmanager.h"
#include "apk/package.h"
#include "apk/packagelistmodel.h"
#include "apk/project.h"
#include "sheets/basefilesheet.h"
#include <QAction>
#include <QBoxLayout>
#include <QEvent>
#include <QMenu>
#include <QMessageBox>
#include <QTabWidget>

ProjectManager::ProjectManager(PackageListModel &packages, QWidget *parent)
    : QWidget(parent)
    , packages(&packages)
{
    setLayout(new QHBoxLayout);
    layout()->setMargin(0);

    actionSavePackage = new QAction(this);
    actionSavePackage->setIcon(QIcon::fromTheme("apk-save"));
    actionSavePackage->setShortcut(QKeySequence("Ctrl+Alt+S"));
    connect(actionSavePackage, &QAction::triggered, this, [this]() {
        currentProject->saveProject();
    });

    actionSaveFile = new QAction(this);
    actionSaveFile->setIcon(QIcon::fromTheme("document-save"));
    connect(actionSaveFile, &QAction::triggered, this, [this]() {
        qobject_cast<BaseEditableSheet *>(currentProject->getCurrentTab())->save();
    });

    actionSaveFileAs = new QAction(this);
    actionSaveFileAs->setIcon(QIcon::fromTheme("document-save-as"));
    connect(actionSaveFileAs, &QAction::triggered, this, [this]() {
        qobject_cast<BaseFileSheet *>(currentProject->getCurrentTab())->saveAs();
    });

    actionInstallPackage = new QAction(this);
    actionInstallPackage->setIcon(QIcon::fromTheme("apk-install"));
    actionInstallPackage->setShortcut(QKeySequence("Ctrl+I"));
    connect(actionInstallPackage, &QAction::triggered, this, [this]() {
        currentProject->installProject();
    });

    actionExplorePackage = new QAction(this);
    actionExplorePackage->setIcon(QIcon::fromTheme("folder-open"));
    actionExplorePackage->setShortcut(QKeySequence("Ctrl+E"));
    connect(actionExplorePackage, &QAction::triggered, this, [this]() {
        currentProject->exploreProject();
    });

    actionClosePackage = new QAction(this);
    actionClosePackage->setIcon(QIcon::fromTheme("document-close"));
    actionClosePackage->setShortcut(QKeySequence("Ctrl+W"));
    connect(actionClosePackage, &QAction::triggered, this, [this]() {
        if (currentProject->isUnsaved()) {
            const QString question = tr("Are you sure you want to close this APK?\nAny unsaved changes will be lost.");
            if (QMessageBox::question(parentWidget(), {}, question) != QMessageBox::Yes) {
                return;
            }
        }
        this->packages->close(currentProject->getPackage());
    });

    actionOpenProjectPage = new QAction(this);
    actionOpenProjectPage->setIcon(QIcon::fromTheme("tool-projectmanager"));
    actionOpenProjectPage->setShortcut(QKeySequence("Ctrl+M"));
    connect(actionOpenProjectPage, &QAction::triggered, this, [this]() {
        currentProject->openProjectTab();
    });

    actionEditTitles = new QAction(this);
    actionEditTitles->setIcon(QIcon::fromTheme("tool-titleeditor"));
    actionEditTitles->setShortcut(QKeySequence("Ctrl+T"));
    connect(actionEditTitles, &QAction::triggered, this, [this]() {
        currentProject->openTitlesTab();
    });

    actionEditPermissions = new QAction(this);
    actionEditPermissions->setIcon(QIcon::fromTheme("tool-permissioneditor"));
    actionEditPermissions->setShortcut(QKeySequence("Ctrl+Shift+P"));
    connect(actionEditPermissions, &QAction::triggered, this, [this]() {
        currentProject->openPermissionEditor();
    });

    actionClonePackage = new QAction(this);
    actionClonePackage->setIcon(QIcon::fromTheme("edit-copy"));
    actionClonePackage->setShortcut(QKeySequence("Ctrl+Shift+R"));
    connect(actionClonePackage, &QAction::triggered, this, [this]() {
        currentProject->openPackageCloner();
    });

    actionSearch = new QAction(this);
    actionSearch->setIcon(QIcon::fromTheme("edit-find"));
    actionSearch->setShortcut(QKeySequence("Ctrl+Shift+F"));
    connect(actionSearch, &QAction::triggered, this, [this]() {
        currentProject->openSearchTab();
    });

    actionViewSignatures = new QAction(this);
    actionViewSignatures->setIcon(QIcon::fromTheme("view-certificate"));
    connect(actionViewSignatures, &QAction::triggered, this, [this]() {
        currentProject->openSignatureViewer();
    });

    menuTab = new QMenu(this);
    menuTab->setEnabled(false);

    onPackageAdded({}, 0, packages.rowCount() - 1);
    connect(&packages, &PackageListModel::rowsInserted, this, &ProjectManager::onPackageAdded);
    connect(&packages, &PackageListModel::rowsAboutToBeRemoved, this, &ProjectManager::onPackageAboutToBeRemoved);

    retranslate();
}

void ProjectManager::setCurrentProject(Project *project)
{
    // Update project widget:

    if (currentWidget) {
        currentWidget->hide();
        layout()->removeWidget(currentWidget);
    }
    currentWidget = project ? project->tabs() : defaultWidget;
    if (currentWidget) {
        currentWidget->show();
        layout()->addWidget(currentWidget);
    }

    // Update actions state:

    updateActionsForProject(project);
    if (currentProject) {
        disconnect(currentProject, &Project::currentTabChanged, this, &ProjectManager::updateActionsForTab);
        disconnect(currentProject->getPackage(), &Package::stateUpdated, this, nullptr);
    }
    currentProject = project;
    if (currentProject) {
        connect(currentProject, &Project::currentTabChanged, this, &ProjectManager::updateActionsForTab);
        connect(currentProject->getPackage(), &Package::stateUpdated, this, [this]() {
            updateActionsForProject(currentProject);
        });
    }
}

void ProjectManager::setCurrentProject(Package *package)
{
    if (package) {
        auto project = projects.value(package);
        Q_ASSERT(project);
        setCurrentProject(project);
    } else {
        setCurrentProject(static_cast<Project *>(nullptr));
    }
}

void ProjectManager::setDefaultWidget(QWidget *defaultWidget)
{
    this->defaultWidget = defaultWidget;
}

Project *ProjectManager::getCurrentProject() const
{
    return currentProject;
}

Project *ProjectManager::hasUnsavedProjects() const
{
    for (auto project : qAsConst(projects)) {
        if (project->isUnsaved()) {
            return project;
        }
    }
    return nullptr;
}

QAction *ProjectManager::getActionSavePackage() const
{
    return actionSavePackage;
}

QAction *ProjectManager::getActionSaveFile() const
{
    return actionSaveFile;
}

QAction *ProjectManager::getActionSaveFileAs() const
{
    return actionSaveFileAs;
}

QAction *ProjectManager::getActionInstallPackage() const
{
    return actionInstallPackage;
}

QAction *ProjectManager::getActionExplorePackage() const
{
    return actionExplorePackage;
}

QAction *ProjectManager::getActionClosePackage() const
{
    return actionClosePackage;
}

QAction *ProjectManager::getActionEditTitles() const
{
    return actionEditTitles;
}

QAction *ProjectManager::getActionEditPermissions() const
{
    return actionEditPermissions;
}

QAction *ProjectManager::getActionClonePackage() const
{
    return actionClonePackage;
}

QAction *ProjectManager::getActionViewSignatures() const
{
    return actionViewSignatures;
}

QAction *ProjectManager::getActionOpenProjectPage() const
{
    return actionOpenProjectPage;
}

QAction *ProjectManager::getActionSearch() const
{
    return actionSearch;
}

QMenu *ProjectManager::getTabMenu() const
{
    return menuTab;
}

void ProjectManager::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    QWidget::changeEvent(event);
}

void ProjectManager::onPackageAdded(const QModelIndex &, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        const auto package = packages->at(i);
        const auto project = new Project(package, this);
        projects.insert(package, project);
        emit projectCreated(project);
    }
}

void ProjectManager::onPackageAboutToBeRemoved(const QModelIndex &, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        const auto package = packages->at(i);
        const auto project = projects.take(package);
        project->deleteLater();
    }
}

void ProjectManager::updateActionsForProject(Project *project)
{
    const auto package = project ? project->getPackage() : nullptr;
    const auto state = package ? &package->getState() : nullptr;
    actionSavePackage->setEnabled(state ? state->canSave() : false);
    actionInstallPackage->setEnabled(state ? state->canInstall() : false);
    actionExplorePackage->setEnabled(state ? state->canExplore() : false);
    actionClosePackage->setEnabled(state ? state->canClose() : false);
    actionEditTitles->setEnabled(state ? state->canEdit() : false);
    actionEditPermissions->setEnabled(state ? state->canEdit() : false);
    actionClonePackage->setEnabled(state ? state->canEdit() : false);
    actionSearch->setEnabled(state ? state->canExplore() : false);
    actionViewSignatures->setEnabled(package);
    actionOpenProjectPage->setEnabled(package);
    updateActionsForTab(project ? project->getCurrentTab() : nullptr);
    emit currentPackageStateChanged(package);
}

void ProjectManager::updateActionsForTab(BaseSheet *tab)
{
    actionSaveFile->setEnabled(qobject_cast<const BaseEditableSheet *>(tab));
    actionSaveFileAs->setEnabled(qobject_cast<const BaseFileSheet *>(tab));
    menuTab->clear();
    if (tab && !tab->actions().isEmpty()) {
        menuTab->setEnabled(true);
        menuTab->addActions(tab->actions());
    } else {
        menuTab->setEnabled(false);
    }
}

void ProjectManager::retranslate()
{
    actionSavePackage->setText(tr("&Save APK..."));
    actionInstallPackage->setText(tr("&Install APK..."));
    //: Displayed as "Open Contents".
    actionExplorePackage->setText(tr("O&pen Contents"));
    actionClosePackage->setText(tr("&Close APK"));
    //: This string refers to a single project (as in "Manager of a project").
    actionOpenProjectPage->setText(tr("&Project Manager"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionEditTitles->setText(tr("Edit Application &Title"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionEditPermissions->setText(tr("Edit Application &Permissions"));
    actionClonePackage->setText(tr("&Clone APK"));
    actionSearch->setText(tr("&Search in Project"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionViewSignatures->setText(tr("View &Signatures"));
    actionSaveFile->setText(tr("&Save"));
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    actionSaveFileAs->setText(tr("Save &As..."));
    //: Displayed as "Tab". Refers to a menu bar (along with File, Edit, View, Window, Help, and similar items).
    menuTab->setTitle(tr("Ta&b"));
}
