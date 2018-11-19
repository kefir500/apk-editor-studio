#include "widgets/projectswidget.h"
#include <QModelIndex>

ProjectsWidget::ProjectsWidget(QWidget *parent) : QWidget(parent)
{
    welcome = new WelcomeTab(this);
    stack = new QStackedWidget(this);
    stack->addWidget(welcome);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(stack);
}

void ProjectsWidget::addProject(Project *project)
{
    if (Q_LIKELY(!map.contains(project))) {
        ProjectTabsWidget *tabs = new ProjectTabsWidget(project, this);
        map.insert(project, tabs);
        stack->addWidget(tabs);
        connect(tabs, &ProjectTabsWidget::currentChanged, [=](int index) {
            Q_UNUSED(index)
            auto tab = qobject_cast<BaseEditor *>(tabs->currentWidget());
            emit tabChanged(tab);
        });
    }
}

bool ProjectsWidget::setCurrentProject(Project *project)
{
    ProjectTabsWidget *tabs = map.value(project, nullptr);
    if (tabs) {
        stack->setCurrentWidget(tabs);
        auto tab = qobject_cast<BaseEditor *>(tabs->currentWidget());
        emit tabChanged(tab);
        return true;
    } else {
        stack->setCurrentWidget(welcome);
        emit tabChanged(welcome);
        return false;
    }
}

void ProjectsWidget::removeProject(Project *project)
{
    delete map.value(project);
    map.remove(project);
}

bool ProjectsWidget::saveCurrentProject()
{
    return getCurrentTabs()->saveProject();
}

bool ProjectsWidget::installCurrentProject()
{
    return getCurrentTabs()->installProject();
}

bool ProjectsWidget::exploreCurrentProject()
{
    return getCurrentTabs()->exploreProject();
}

bool ProjectsWidget::closeCurrentProject()
{
    return getCurrentTabs()->closeProject();
}

ProjectManager *ProjectsWidget::openProjectTab()
{
    return getCurrentTabs()->openProjectTab();
}

TitleEditor *ProjectsWidget::openTitlesTab()
{
    return getCurrentTabs()->openTitlesTab();
}

BaseEditor *ProjectsWidget::openResourceTab(const QModelIndex &index)
{
    return getCurrentTabs()->openResourceTab(index);
}

Project *ProjectsWidget::hasUnsavedProjects() const
{
    QMapIterator<Project *, ProjectTabsWidget *> it(map);
    while (it.hasNext()) {
        it.next();
        auto tabs = it.value();
        if (tabs->isUnsaved()) {
            Project *project = it.key();
            return project;
        }
    }
    return false;
}

Project *ProjectsWidget::getCurrentProject() const
{
    return map.key(getCurrentTabs(), nullptr);
}

ProjectTabsWidget *ProjectsWidget::getCurrentTabs() const
{
    return qobject_cast<ProjectTabsWidget *>(stack->currentWidget());
}
