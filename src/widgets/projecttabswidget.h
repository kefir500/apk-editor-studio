#ifndef PROJECTTABSWIDGET_H
#define PROJECTTABSWIDGET_H

#include "apk/resourcemodelindex.h"
#include "editors/projectviewer.h"
#include "editors/titleeditor.h"
#include <QTabWidget>

class ProjectTabsWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit ProjectTabsWidget(Project *project, QWidget *parent = nullptr);

    ProjectViewer *openProjectTab();
    TitleEditor *openTitlesTab();
    Viewer *openResourceTab(const ResourceModelIndex &index);

    bool saveTabs();
    bool isUnsaved() const;

    bool saveProject();
    bool installProject();
    bool exploreProject();
    bool closeProject();

private:
    int addTab(Viewer *tab);
    bool closeTab(Viewer *editor);
    bool hasUnsavedTabs() const;
    Viewer *getTabByIdentifier(const QString &identifier) const;

    Project *project;
};

#endif // PROJECTTABSWIDGET_H
