#ifndef PROJECTITEMSMODEL_H
#define PROJECTITEMSMODEL_H

#include "apk/project.h"

class ProjectItemsModel : public QAbstractListModel
{
    Q_OBJECT

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

    Project *existing(const QString &filename) const;
    int indexOf(Project *project) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

signals:
    void added(Project *project) const;
    void changed(Project *project) const;
    void removed(Project *project) const;

private:
    QList<Project *> projects;
};

#endif // PROJECTITEMSMODEL_H
