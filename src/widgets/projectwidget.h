#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include "apk/resourcemodelindex.h"
#include <QTabWidget>

class Project;
class ProjectItemsModel;
class Viewer;

class ProjectWidget : public QTabWidget
{
    Q_OBJECT

public:
    ProjectWidget(Project *project, ProjectItemsModel &projects, QWidget *parent = nullptr);

    void openProjectTab();
    void openTitlesTab();
    void openResourceTab(const ResourceModelIndex &index);
    void openPermissionEditor();
    bool openPackageRenamer();

    bool saveTabs();
    bool isUnsaved() const;

    bool saveProject();
    bool installProject();
    bool exploreProject();
    bool closeProject();

signals:
    void currentTabChanged(Viewer *viewer);

private:
    int addTab(Viewer *tab);
    bool closeTab(Viewer *tab);
    bool hasUnsavedTabs() const;
    Viewer *getTabByIdentifier(const QString &identifier) const;

    Project *project;
    ProjectItemsModel &projects;
};

#endif // PROJECTWIDGET_H
