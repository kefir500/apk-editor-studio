#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include "editors/projectmanager.h"
#include "editors/titleeditor.h"
#include "editors/welcometab.h"
#include <QTabWidget>

class ProjectWidget : public QTabWidget
{
    Q_OBJECT

public:
    ProjectWidget(Project *project, QWidget *parent = nullptr);

    BaseEditor *currentTab() const;

    bool saveProject();
    bool installProject();
    bool exploreProject();
    bool closeProject();

    ProjectManager *openProjectTab();
    TitleEditor *openTitlesTab();
    BaseEditor *openResourceTab(const QString &path, const QPixmap &icon = QPixmap());

    bool saveTabs();
    bool closeTab(BaseEditor *editor);

    bool hasUnsavedProject() const;
    bool hasUnsavedTabs() const;

    Project *getProject() const;

private:
    int addTab(BaseEditor *tab);

    BaseEditor *getTabByIdentifier(const QString &identifier);

    Project *project;
};

#endif // PROJECTWIDGET_H
