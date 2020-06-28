#ifndef PROJECTITEMSMODEL_H
#define PROJECTITEMSMODEL_H

#include "apk/project.h"

class ProjectItemsModel : public QAbstractListModel
{
public:
    enum Column {
        TitleColumn,
        OriginalPathColumn,
        ContentsPathColumn,
        StatusColumn,
        IsUnpackedColumn,
        IsModifiedColumn,
        ColumnCount
    };

    ~ProjectItemsModel() override;

    Project *add(const QString &path, QWidget *parent);
    bool close(Project *project);

    Project *at(int row) const;
    Project *existing(const QString &filename) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QList<Project *> projects;
};

#endif // PROJECTITEMSMODEL_H
