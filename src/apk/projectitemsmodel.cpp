#include "apk/projectitemsmodel.h"
#include "apk/project.h"
#include "windows/mainwindow.h"
#include <QMessageBox>

ProjectItemsModel::~ProjectItemsModel()
{
    qDeleteAll(projects);
}

Project *ProjectItemsModel::add(const QString &path, MainWindow *window)
{
    Project *existing = this->existing(path);
    if (existing) {
        //: "%1" will be replaced with a path to an APK.
        const QString question = tr("This APK is already open:\n%1\nDo you want to reopen it and lose any unsaved changes?").arg(existing->getOriginalPath());
        const int answer = QMessageBox::question(window, QString(), question);
        if (answer != QMessageBox::Yes) {
            return nullptr;
        }
        close(existing);
    }
    auto project = new Project(path);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
        projects.append(project);
    endInsertRows();

    connect(project, &Project::stateUpdated, this, [=]() {
        const int row = projects.indexOf(project);
        emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
    });

    if (window) {
        window->setCurrentProject(project);
    }

    return project;
}

bool ProjectItemsModel::close(Project *project)
{
    const int row = projects.indexOf(project);
    if (row == -1) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
        projects.removeAt(row);
        project->deleteLater();
    endRemoveRows();

    return true;
}

Project *ProjectItemsModel::at(int row) const
{
    return projects.at(row);
}

Project *ProjectItemsModel::existing(const QString &filename) const
{
    QListIterator<Project *> it(projects);
    while (it.hasNext()) {
        Project *iteration = it.next();
        if (iteration->getOriginalPath() == QDir::toNativeSeparators(filename)) {
            return iteration;
        }
    }
    return nullptr;
}

QVariant ProjectItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const int column = index.column();
        const Project *project = static_cast<Project *>(index.internalPointer());
        if (role == Qt::DisplayRole) {
            switch (column) {
            case TitleColumn:
                return project->getTitle();
            case OriginalPathColumn:
                return project->getOriginalPath();
            case ContentsPathColumn:
                return project->getContentsPath();
            case IsUnpackedColumn:
                return project->getState().isUnpacked();
            case IsModifiedColumn:
                return project->getState().isModified();
            }
        } else if (role == Qt::DecorationRole) {
            switch (column) {
            case TitleColumn:
                return project->getThumbnail();
            case StatusColumn:
                switch (project->getState().getCurrentStatus()) {
                case ProjectState::Status::Normal:
                    return QIcon::fromTheme("apk-idle");
                case ProjectState::Status::Unpacking:
                    return QIcon::fromTheme("apk-opening");
                case ProjectState::Status::Packing:
                case ProjectState::Status::Signing:
                case ProjectState::Status::Optimizing:
                    return QIcon::fromTheme("apk-saving");
                case ProjectState::Status::Installing:
                    return QIcon::fromTheme("apk-installing");
                case ProjectState::Status::Errored:
                    return QIcon::fromTheme("apk-error");
                }
            }
        }
    }
    return QVariant();
}

QModelIndex ProjectItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row >= 0 && row < projects.count()) {
        return createIndex(row, column, projects.at(row));
    }
    return QModelIndex();
}

int ProjectItemsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return projects.count();
}
