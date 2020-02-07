#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "apk/project.h"
#include "widgets/logview.h"
#include "widgets/manifestview.h"
#include "widgets/projectswidget.h"
#include "widgets/resourceabstractview.h"
#include "widgets/projectlist.h"
#include "widgets/toolbar.h"
#include <QMainWindow>
#include <QActionGroup>
#include <QDragEnterEvent>
#include <QDropEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

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

    void loadSettings();
    void resetSettings();
    void saveSettings();

    Viewer *openResource(const QModelIndex &index);

    bool setCurrentProject(Project *project);
    void setActionsEnabled(const Project *project);
    void updateWindowForProject(const Project *project);

    ProjectsWidget *projectsWidget;

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
    QAction *actionApkInstallExternal;
    QAction *actionApkExplore;
    QAction *actionApkClose;
    QAction *actionProjectManager;
    QAction *actionTitleEditor;
    QAction *actionPermissionEditor;
    QAction *actionSettingsReset;
    QAction *actionAboutQt;
    QAction *actionAbout;
    QActionGroup *actionsLanguage;

    QRubberBand *rubberBand;

    QByteArray defaultState;
};

#endif // MAINWINDOW_H
