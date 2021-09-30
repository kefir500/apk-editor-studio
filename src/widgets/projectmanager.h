#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QMap>
#include <QWidget>

class BaseSheet;
class Package;
class PackageListModel;
class Project;
class QMenu;

class ProjectManager : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectManager(PackageListModel &packages, QWidget *parent = nullptr);

    void setCurrentProject(Project *project);
    void setCurrentProject(Package *package);
    void setDefaultWidget(QWidget *widget);

    Project *getCurrentProject() const;
    Project *hasUnsavedProjects() const;

    QAction *getActionSavePackage() const;
    QAction *getActionSaveFile() const;
    QAction *getActionSaveFileAs() const;
    QAction *getActionInstallPackage() const;
    QAction *getActionExplorePackage() const;
    QAction *getActionClosePackage() const;
    QAction *getActionEditTitles() const;
    QAction *getActionEditPermissions() const;
    QAction *getActionClonePackage() const;
    QAction *getActionViewSignatures() const;
    QAction *getActionOpenProjectPage() const;
    QMenu *getTabMenu() const;

signals:
    void projectCreated(Project *project);
    void currentPackageStateChanged(Package *package);

protected:
    void changeEvent(QEvent *event) override;

private:
    void onPackageAdded(const QModelIndex &parent, int first, int last);
    void onPackageAboutToBeRemoved(const QModelIndex &parent, int first, int last);

    void updateActionsForProject(Project *project);
    void updateActionsForTab(BaseSheet *tab);
    void retranslate();

    QMap<Package *, Project *> projects;
    PackageListModel *packages;
    Project *currentProject = nullptr;
    QWidget *currentWidget = nullptr;
    QWidget *defaultWidget = nullptr;

    QAction *actionSavePackage;
    QAction *actionSaveFile;
    QAction *actionSaveFileAs;
    QAction *actionInstallPackage;
    QAction *actionExplorePackage;
    QAction *actionClosePackage;
    QAction *actionEditTitles;
    QAction *actionEditPermissions;
    QAction *actionClonePackage;
    QAction *actionViewSignatures;
    QAction *actionOpenProjectPage;
    QMenu *menuTab;
};

#endif // PROJECTMANAGER_H
