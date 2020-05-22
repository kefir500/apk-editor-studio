#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include "apk/resourceitemsmodel.h"
#include <QFileSystemModel>

class FileSystemModel : public QFileSystemModel, public IResourceItemsModel
{
    Q_OBJECT
    Q_INTERFACES(IResourceItemsModel)

public:
    explicit FileSystemModel(QObject *parent = nullptr) :
        QFileSystemModel(parent), sourceModel(nullptr) {}

    void setSourceModel(ResourceItemsModel *model);
    QModelIndex rootIndex() const;

    bool replaceResource(const QModelIndex &index, const QString &file = QString()) override;
    bool removeResource(const QModelIndex &index) override;
    QString getResourcePath(const QModelIndex &index) const override;

    bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
    void updated(const QModelIndex &from, const QModelIndex &to, const QVector<int> &roles = {});

    ResourceItemsModel *sourceModel;
};

#endif // FILESYSTEMMODEL_H
