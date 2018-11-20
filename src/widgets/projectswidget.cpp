#include "widgets/projectswidget.h"
#include "editors/fileeditor.h"
#include "base/application.h"
#include <QModelIndex>

ProjectsWidget::ProjectsWidget(QWidget *parent) : QWidget(parent)
{
    welcome = new WelcomeTab(this);
    stack = new QStackedWidget(this);
    stack->addWidget(welcome);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(stack);

    actionSave = new QAction(app->loadIcon("save.png"), QString(), this);
    actionSaveAs = new QAction(app->loadIcon("save-as.png"), QString(), this);
    actionSave->setEnabled(false);
    actionSaveAs->setEnabled(false);
    actionNone = new QAction(QString(), this);
    actionNone->setEnabled(false);
    addAction(actionSave);
    addAction(actionSaveAs);
    Toolbar::addToPool("save", actionSave);
    Toolbar::addToPool("save-as", actionSaveAs);

    connect(this, &ProjectsWidget::tabChanged, [=](BaseEditor *tab) {
        const bool isEditor = qobject_cast<SaveableEditor *>(tab);
        const bool isFileEditor = qobject_cast<FileEditor *>(tab);
        actionSave->setEnabled(isEditor);
        actionSaveAs->setEnabled(isFileEditor);
    });
    connect(actionSave, &QAction::triggered, this, &ProjectsWidget::saveCurrentTab);
    connect(actionSaveAs, &QAction::triggered, this, &ProjectsWidget::saveCurrentTabAs);

    retranslate();
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
    return getCurrentProjectTabs()->saveProject();
}

bool ProjectsWidget::installCurrentProject()
{
    return getCurrentProjectTabs()->installProject();
}

bool ProjectsWidget::exploreCurrentProject()
{
    return getCurrentProjectTabs()->exploreProject();
}

bool ProjectsWidget::closeCurrentProject()
{
    return getCurrentProjectTabs()->closeProject();
}

bool ProjectsWidget::saveCurrentTab()
{
    auto saveableEditor = qobject_cast<SaveableEditor *>(getCurrentTab());
    if (saveableEditor) {
        return saveableEditor->save();
    }
    return false;
}

bool ProjectsWidget::saveCurrentTabAs()
{
    auto fileEditor = qobject_cast<FileEditor *>(getCurrentTab());
    if (fileEditor) {
        return fileEditor->saveAs();
    }
    return false;
}

ProjectManager *ProjectsWidget::openProjectTab()
{
    return getCurrentProjectTabs()->openProjectTab();
}

TitleEditor *ProjectsWidget::openTitlesTab()
{
    return getCurrentProjectTabs()->openTitlesTab();
}

BaseEditor *ProjectsWidget::openResourceTab(const QModelIndex &index)
{
    return getCurrentProjectTabs()->openResourceTab(index);
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
    return nullptr;
}

Project *ProjectsWidget::getCurrentProject() const
{
    return map.key(getCurrentProjectTabs(), nullptr);
}

ProjectTabsWidget *ProjectsWidget::getCurrentProjectTabs() const
{
    return qobject_cast<ProjectTabsWidget *>(stack->currentWidget());
}

QList<QAction *> ProjectsWidget::getCurrentTabActions() const
{
    auto tab = getCurrentTab();
    return tab ? tab->actions() : QList<QAction *>({actionNone});
}

void ProjectsWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        QWidget::changeEvent(event);
    }
}

BaseEditor *ProjectsWidget::getCurrentTab() const
{
    auto tabs = getCurrentProjectTabs();
    return tabs ? qobject_cast<BaseEditor *>(tabs->currentWidget()) : welcome;
}

void ProjectsWidget::retranslate()
{
    actionSave->setText(tr("&Save"));
    actionSaveAs->setText(tr("Save &As"));
    actionNone->setText(tr("No actions"));
}
