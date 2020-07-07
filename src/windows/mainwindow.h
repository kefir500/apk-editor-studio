#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>
#include <QRubberBand>

class ExtraListItemProxy;
class LogView;
class CentralWidget;
class ManifestView;
class Project;
class ProjectList;
class ProjectWidget;
class ResourceAbstractView;
class Toolbar;
class Viewer;
class WelcomeActionViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    void updateWindowForTab(Viewer *tab);

    void onProjectAdded(const QModelIndex &parent, int first, int last);
    void onProjectAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onProjectSwitched(Project *project);

    Project *getCurrentProject() const;
    ProjectWidget *getCurrentProjectWidget() const;
    Viewer *getCurrentTab() const;

    CentralWidget *centralWidget;
    ProjectList *projectList;
    LogView *logView;
    ManifestView *manifestTable;
    ResourceAbstractView *resourceTree;
    ResourceAbstractView *filesystemTree;
    ResourceAbstractView *iconList;

    Toolbar *toolbar;
    QDockWidget *dockProjects;
    QDockWidget *dockResources;
    QDockWidget *dockFilesystem;
    QDockWidget *dockManifest;
    QDockWidget *dockIcons;

    QMenu *menuFile;
    QMenu *menuEditor;
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
    QAction *actionProjectManager;
    QAction *actionTitleEditor;
    QAction *actionPermissionEditor;
    QAction *actionAboutQt;
    QAction *actionAbout;
    QActionGroup *actionsLanguage;

    QRubberBand *rubberBand;

    QByteArray defaultState;

    QMap<Project *, ProjectWidget *> projectWidgets;
    WelcomeActionViewer *welcomePage;
    ExtraListItemProxy *welcomeItemProxy;

    static int instances;
};

#endif // MAINWINDOW_H
