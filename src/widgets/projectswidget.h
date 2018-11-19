#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include "widgets/projecttabswidget.h"
#include "editors/welcometab.h"
#include <QStackedWidget>

class ProjectsWidget : public QWidget
{
    Q_OBJECT

public:
    ProjectsWidget(QWidget *parent = nullptr);

    void addProject(Project *project);
    bool setCurrentProject(Project *project);
    void removeProject(Project *project);

    bool saveCurrentProject();
    bool installCurrentProject();
    bool exploreCurrentProject();
    bool closeCurrentProject();

    ProjectManager *openProjectTab();
    TitleEditor *openTitlesTab();
    BaseEditor *openResourceTab(const QModelIndex &index);

    Project *hasUnsavedProjects() const;
    Project *getCurrentProject() const;
    ProjectTabsWidget *getCurrentTabs() const;

signals:
    void tabChanged(BaseEditor *tab) const;

private:
    QStackedWidget *stack;
    QMap<Project *, ProjectTabsWidget *> map;
    WelcomeTab *welcome;
};

#endif // PROJECTSWIDGET_H
