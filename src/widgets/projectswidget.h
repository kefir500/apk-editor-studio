#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include "widgets/projecttabswidget.h"
#include "apk/projectsmodel.h"
#include "editors/welcomeviewer.h"
#include <QStackedLayout>

class ProjectsWidget : public QWidget
{
    Q_OBJECT

public:
    ProjectsWidget(QWidget *parent = nullptr);

    void setModel(ProjectsModel *model);

    void addProject(Project *project);
    bool setCurrentProject(Project *project);
    void removeProject(Project *project);

    bool saveCurrentProject();
    bool installCurrentProject();
    bool exploreCurrentProject();
    bool closeCurrentProject();

    bool saveCurrentTab();
    bool saveCurrentTabAs();

    ProjectViewer *openProjectTab();
    TitleEditor *openTitlesTab();
    Viewer *openResourceTab(const QModelIndex &index);

    bool hasUnsavedProjects();
    Project *getCurrentProject() const;
    QList<QAction *> getCurrentTabActions() const;

signals:
    void currentProjectChanged(Project *project) const;
    void currentTabChanged(Viewer *tab) const;

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    ProjectTabsWidget *getCurrentProjectTabs() const;
    Viewer *getCurrentProjectTab() const;

    void retranslate();

    ProjectsModel *model;
    QMap<Project *, ProjectTabsWidget *> map;

    QStackedLayout *stack;
    WelcomeViewer *welcome;

    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionNone;
};

#endif // PROJECTSWIDGET_H
