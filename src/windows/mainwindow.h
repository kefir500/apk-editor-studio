#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "apk/project.h"
#include "widgets/logview.h"
#include "widgets/manifestview.h"
#include "widgets/projectswidget.h"
#include "widgets/resourcesview.h"
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

    BaseEditor *openResource(const QModelIndex &index);
    BaseEditor *openManifestEditor(ManifestModel::ManifestRow manifestField);
    void openLogEntry(const QModelIndex &index);

    bool setCurrentProject(Project *project);
    void setActionsEnabled(const Project *project);
    void updateWindowForProject(const Project *project);
    void updateRecentMenu();

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void setInitialSize();
    void initWidgets();
    void initMenus();
    void initLanguages();
    void retranslate();

    void loadSettings();
    void resetSettings();
    void saveSettings();

    ProjectsWidget *projectsWidget;

    ProjectList *projectsList;
    LogView *logView;
    ManifestView *manifestTable;
    ResourcesView *resourcesTree;
    ResourcesView *filesystemTree;
    ResourcesView *iconsList;

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
    QMenu *menuRecent;
    QMenu *menuLanguage;
    QAction *actionApkOpen;
    QAction *actionApkSave;
    QAction *actionApkExplore;
    QAction *actionApkInstall;
    QAction *actionApkClose;
    QAction *actionExit;
    QAction *actionRecentClear;
    QAction *actionRecentNone;
    QAction *actionKeyManager;
    QAction *actionDeviceManager;
    QAction *actionProjectManager;
    QAction *actionTitleEditor;
    QAction *actionOptions;
    QAction *actionSettingsReset;
    QAction *actionWebsite;
    QAction *actionUpdate;
    QAction *actionDonate;
    QAction *actionAboutQt;
    QAction *actionAbout;
    QActionGroup *actionsLanguage;

    QByteArray defaultState;
};

#endif // MAINWINDOW_H
