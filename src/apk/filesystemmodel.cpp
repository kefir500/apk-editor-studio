#include "apk/filesystemmodel.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

void FileSystemModel::setSourceModel(ResourceItemsModel *model)
{
    sourceModel = model;
}

bool FileSystemModel::replaceResource(const QModelIndex &index, const QString &file)
{
    if (!sourceModel) {
        return false;
    }
    const auto path = filePath(index);
    const auto resourceIndex = sourceModel->findIndex(path);
    return sourceModel->replaceResource(resourceIndex, file);
}

bool FileSystemModel::removeResource(const QModelIndex &index)
{
    return removeRow(index.row(), index.parent());
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case PathRole: return filePath(index);
    case IconRole: return fileIcon(index);
    default:
        return QFileSystemModel::data(index, role);
    }
}

bool FileSystemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count)
    if (!sourceModel) {
        return false;
    }
    bool success = true;
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        const auto path = filePath(index(row, 0, parent));
        const auto resourceIndex = sourceModel->findIndex(path);
        if (!sourceModel->removeResource(resourceIndex)) {
            success = false;
        }
    }
    endRemoveRows();
    return success;
}
