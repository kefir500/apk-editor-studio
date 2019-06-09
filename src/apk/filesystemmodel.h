#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include "apk/resourceitemsmodel.h"
#include <QFileSystemModel>

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit FileSystemModel(ResourceItemsModel *resourceModel, QObject *parent = nullptr) :
        QFileSystemModel(parent), resourceModel(resourceModel) {}

    bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
    ResourceItemsModel *resourceModel;
};

#endif // FILESYSTEMMODEL_H
