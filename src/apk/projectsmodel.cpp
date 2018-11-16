#include "apk/projectsmodel.h"
#include "base/application.h"

ProjectsModel::~ProjectsModel()
{
    qDeleteAll(projects);
}

Project *ProjectsModel::open(const QString &filename, bool unpack)
{
    Project *project = new Project(filename);
    if (unpack) {
        project->unpack();
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
        projects.append(project);
        emit added(project);
    endInsertRows();

    connect(project, &Project::dataChanged, [=]() {
        const int row = indexOf(project);
        emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
    });

    return project;
}

bool ProjectsModel::close(Project *project)
{
    const int row = indexOf(project);
    if (row == -1) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
        projects.removeAt(row);
        delete project;
    endRemoveRows();

    emit removed(project);
    return true;
}

Project *ProjectsModel::get(const QString &filename) const
{
    QListIterator<Project *> it(projects);
    while (it.hasNext()) {
        Project *iteration = it.next();
        if (iteration->getOriginalPath() == filename) {
            return iteration;
        }
    }
    return nullptr;
}

int ProjectsModel::indexOf(Project *project) const
{
    return projects.indexOf(project);
}

QVariant ProjectsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const int column = index.column();
        const Project *project = static_cast<Project *>(index.internalPointer());
        if (role == Qt::DisplayRole) {
            switch (column) {
                case ProjectTitle:        return project->getTitle();
                case ProjectOriginalPath: return project->getOriginalPath();
                case ProjectContentsPath: return project->getContentsPath();
            }
        } else if (role == Qt::DecorationRole) {
            switch (column) {
                case ProjectTitle:
                    QIcon thumbnail = project->getThumbnail();
                    return !thumbnail.isNull() ? thumbnail : app->loadPixmap("loading.png");
            }
        } else if (role == ProjectStateRole) {
            return project->getState();
        } else if (role == ProjectErrorRole) {
            return project->getErroredState();
        }
    }
    return QVariant();
}

QModelIndex ProjectsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row >= 0 && row < projects.count()) {
        return createIndex(row, column, projects.at(row));
    }
    return QModelIndex();
}

int ProjectsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return projects.count();
}
