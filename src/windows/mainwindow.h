#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "apk/project.h"
#include "widgets/iconlist.h"
#include "widgets/logview.h"
#include "widgets/manifestview.h"
#include "widgets/projectwidget.h"
#include "widgets/toolbar.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QTreeView>
#include <QComboBox>
#include <QActionGroup>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QSortFilterProxyModel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    BaseEditor *openResourceItem(const QModelIndex &index);
    BaseEditor *openIconItem(const QModelIndex &index);
    BaseEditor *openFilesystemItem(const QModelIndex &index);
    BaseEditor *openManifestEditor(ManifestModel::ManifestRow manifestField);
    void openLogEntry(const QModelIndex &index);

    void onProjectAdded(Project *project);
    void onProjectRemoved(Project *project);
    void setCurrentProject(Project *project);
    void setCurrentProject(int index);
    void hello(const Project *project);
    void setActionsEnabled(const Project *project);

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

    Project *getCurrentProject() const;
    ProjectWidget *getCurrentProjectWidget() const;

    QMap<Project *, ProjectWidget *> projectWidgets;
    WelcomeTab *welcome;

    QStackedWidget *tabs;
    QComboBox *projectsList;
    LogView *logView;
    QTreeView *resourcesTree;
    QTreeView *filesystemTree;
    ManifestView *manifestTable;
    IconList *iconsList;

    Toolbar *toolbar;
    QDockWidget *dockProjects;
    QDockWidget *dockResources;
    QDockWidget *dockFilesystem;
    QDockWidget *dockManifest;
    QDockWidget *dockIcons;

    QMenu *menuFile;
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
    QAction *actionResourceSave;
    QAction *actionResourceSaveAs;
    QAction *actionExit;
    QAction *actionWebsite;
    QAction *actionUpdate;
    QAction *actionDonate;
    QAction *actionAbout;
    QAction *actionRecentClear;
    QAction *actionRecentNone;
    QAction *actionKeyManager;
    QAction *actionDeviceManager;
    QAction *actionProjectManager;
    QAction *actionTitleEditor;
    QAction *actionAssociate;
    QAction *actionOptions;
    QAction *actionSettingsReset;
    QAction *actionAboutQt;
    QActionGroup *actionsLanguage;

    QByteArray defaultState;
};

#endif // MAINWINDOW_H
