#ifndef PROJECTSMODEL_H
#define PROJECTSMODEL_H

#include "apk/project.h"

class ProjectsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ProjectColumn {
        ProjectTitle,
        ProjectOriginalPath,
        ProjectContentsPath,
        ColumnCount
    };

    enum ProjectRole {
        ProjectStateRole = Qt::UserRole + 1,
        ProjectErrorRole
    };

    ~ProjectsModel();

    Project *open(const QString &filename, bool unpack = true);
    bool close(Project *project);

    Project *get(const QString &filename) const;
    int indexOf(Project *project) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

signals:
    void added(Project *project) const;
    void removed(Project *project) const;

private:
    QList<Project *> projects;
};

#endif // PROJECTSMODEL_H
