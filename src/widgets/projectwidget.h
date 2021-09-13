#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QTabWidget>

class BaseSheet;
class Project;
class ProjectItemsModel;
class ResourceModelIndex;

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
    void currentTabChanged(BaseSheet *sheet);

private:
    int addTab(BaseSheet *tab);
    bool closeTab(BaseSheet *tab);
    bool hasUnsavedTabs() const;
    BaseSheet *getTabByIdentifier(const QString &identifier) const;

    Project *project;
    ProjectItemsModel &projects;
};

#endif // PROJECTWIDGET_H
