#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class ProjectManager;
class ExtraListItemProxy;
class FileSystemModel;
class LogView;
class ManifestView;
class Package;
class PackageListModel;
class Project;
class PackageList;
class QActionGroup;
class QDragEnterEvent;
class QDropEvent;
class QLineEdit;
class QRubberBand;
class ResourceAbstractView;
class ResourceItemsModel;
class Toolbar;
class UpdateDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(PackageListModel &packages, QWidget *parent = nullptr);
    ~MainWindow();

    void openApk(const QString &path);
    void openExternalApk();
    void optimizeExternalApk();
    void signExternalApk();
    void installExternalApk();

    void processArguments(const QStringList &arguments);

    void setCurrentPackage(Package *package);

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
    void checkToolsAvailable();
    void retranslate();

    Package *addPackage(const QString &path);

    void updateWindowForPackage(Package *package);
    void updateRecentMenu();
    void onPackageSwitched(Package *package);

    Project *getCurrentProject() const;

    static int instances;

    PackageListModel &packages;
    ExtraListItemProxy *welcomeItemProxy;
    QByteArray defaultState;

    // Used to display resource view headers even without project
    ResourceItemsModel *dummyResourceModel;
    FileSystemModel *dummyFileSystemModel;

    UpdateDialog *updateDialog;
    ProjectManager *projectManager;
    PackageList *packageList;
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
    QMenu *menuTools;
    QMenu *menuSettings;
    QMenu *menuWindow;
    QMenu *menuHelp;
    QAction *actionRecentClear;
    QAction *actionRecentNone;
    QAction *actionNewWindow;
    QAction *actionCheckUpdates;
    QAction *actionAboutQt;
    QAction *actionAbout;
    QActionGroup *actionsLanguage;
    QRubberBand *rubberBand;
};

#endif // MAINWINDOW_H
