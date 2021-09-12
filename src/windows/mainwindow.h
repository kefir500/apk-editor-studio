#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class BaseSheet;
class CentralWidget;
class ExtraListItemProxy;
class FileSystemModel;
class LogView;
class ManifestView;
class Project;
class ProjectItemsModel;
class ProjectList;
class ProjectWidget;
class QActionGroup;
class QDragEnterEvent;
class QDropEvent;
class QLineEdit;
class QRubberBand;
class ResourceAbstractView;
class ResourceItemsModel;
class Toolbar;
class WelcomeSheet;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(ProjectItemsModel &projects, QWidget *parent = nullptr);
    ~MainWindow();

    void openApk(const QString &path);
    void openExternalApk();
    void optimizeExternalApk();
    void signExternalApk();
    void installExternalApk();

    void processArguments(const QStringList &arguments);

    void setCurrentProject(Project *project);

protected:
    void changeEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setInitialSize();
    void initWidgets();
    void initMenus();
    void retranslate();

    void updateWindowForProject(Project *project);
    void updateWindowForTab(BaseSheet *tab);
    void updateRecentMenu();

    void onProjectAdded(const QModelIndex &parent, int first, int last);
    void onProjectAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onProjectSwitched(Project *project);

    Project *addProject(const QString &path);
    Project *getCurrentProject() const;
    ProjectWidget *getCurrentProjectWidget() const;
    BaseSheet *getCurrentTab() const;

    static int instances;
    ProjectItemsModel &projects;

    CentralWidget *centralWidget;
    ProjectList *projectList;
    LogView *logView;
    ManifestView *manifestTable;
    ResourceAbstractView *resourceTree;
    QLineEdit *resourceFilterInput;
    ResourceAbstractView *filesystemTree;
    ResourceAbstractView *iconList;
    Toolbar *toolbar;
    QDockWidget *dockProjects;
    QDockWidget *dockResources;
    QDockWidget *dockFilesystem;
    QDockWidget *dockManifest;
    QDockWidget *dockIcons;
    QMenu *menuFile;
    QMenu *menuRecent;
    QMenu *menuTab;
    QMenu *menuTools;
    QMenu *menuSettings;
    QMenu *menuWindow;
    QMenu *menuHelp;
    QAction *actionApkSave;
    QAction *actionApkInstall;
    QAction *actionApkExplore;
    QAction *actionApkClose;
    QAction *actionFileSave;
    QAction *actionFileSaveAs;
    QAction *actionRecentClear;
    QAction *actionRecentNone;
    QAction *actionProjectManager;
    QAction *actionTitleEditor;
    QAction *actionPermissionEditor;
    QAction *actionViewSignatures;
    QAction *actionCloneApk;
    QAction *actionNewWindow;
    QAction *actionAboutQt;
    QAction *actionAbout;
    QActionGroup *actionsLanguage;
    QRubberBand *rubberBand;

    QMap<Project *, ProjectWidget *> projectWidgets;
    WelcomeSheet *welcomePage;
    ExtraListItemProxy *welcomeItemProxy;

    QByteArray defaultState;

    // Used to display resource view headers even without project
    ResourceItemsModel *dummyResourceModel;
    FileSystemModel *dummyFileSystemModel;
};

#endif // MAINWINDOW_H
