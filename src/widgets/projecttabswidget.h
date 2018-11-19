#ifndef PROJECTTABSWIDGET_H
#define PROJECTTABSWIDGET_H

#include "apk/resourcemodelindex.h"
#include "editors/projectmanager.h"
#include "editors/titleeditor.h"
#include <QTabWidget>

class ProjectTabsWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit ProjectTabsWidget(Project *project, QWidget *parent = nullptr);

    ProjectManager *openProjectTab();
    TitleEditor *openTitlesTab();
    BaseEditor *openResourceTab(const ResourceModelIndex &index);

    bool saveTabs();
    bool isUnsaved() const;

    bool saveProject();
    bool installProject();
    bool exploreProject();
    bool closeProject();

private:
    int addTab(BaseEditor *tab);    
    bool closeTab(BaseEditor *editor);
    bool hasUnsavedTabs() const;
    BaseEditor *getTabByIdentifier(const QString &identifier) const;

    Project *project;
};

#endif // PROJECTTABSWIDGET_H
