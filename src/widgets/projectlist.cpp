#include "widgets/projectlist.h"
#include "widgets/projectlistitemdelegate.h"
#include "base/application.h"

ProjectList::ProjectList(QWidget *parent) : QComboBox(parent)
{
#if defined(Q_OS_WIN)
    setMinimumHeight(app->scale(40));
    setIconSize(app->scale(32, 32));
#elif defined(Q_OS_OSX)
    setIconSize(app->scale(16, 16));
#else
    setMinimumHeight(app->scale(46));
    setIconSize(app->scale(32, 32));
#endif
    setItemDelegate(new ProjectListItemDelegate(this));

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
        if (model()) {
            Project *project = static_cast<Project *>(model()->index(index, 0).internalPointer());
            emit currentProjectChanged(project);
        }
    });
}

bool ProjectList::setCurrentProject(Project *project)
{
    const int index = model()->indexOf(project);
    if (index >= 0) {
        setCurrentIndex(index);
        return true;
    }
    return false;
}

ProjectsModel *ProjectList::model() const
{
    return static_cast<ProjectsModel *>(QComboBox::model());
}

void ProjectList::setModel(ProjectsModel *model)
{
    QComboBox::setModel(model);
}
