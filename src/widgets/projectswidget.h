#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include "widgets/projecttabswidget.h"
#include "apk/projectsmodel.h"
#include "editors/welcometab.h"
#include <QStackedWidget>

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

    ProjectManager *openProjectTab();
    TitleEditor *openTitlesTab();
    BaseEditor *openResourceTab(const QModelIndex &index);

    Project *hasUnsavedProjects() const;
    Project *getCurrentProject() const;
    ProjectTabsWidget *getCurrentProjectTabs() const;
    QList<QAction *> getCurrentTabActions() const;

signals:
    void tabChanged(BaseEditor *tab) const;

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    BaseEditor *getCurrentTab() const;
    void retranslate();

    QStackedWidget *stack;
    QMap<Project *, ProjectTabsWidget *> map;
    WelcomeTab *welcome;

    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionNone;

    ProjectsModel *model;
};

#endif // PROJECTSWIDGET_H
