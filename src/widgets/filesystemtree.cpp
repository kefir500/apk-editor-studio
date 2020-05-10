#include "widgets/filesystemtree.h"

FileSystemModel *FileSystemTree::model() const
{
    return static_cast<FileSystemModel *>(QTreeView::model());
}

void FileSystemTree::setModel(QAbstractItemModel *newModel)
{
    const auto oldModel = qobject_cast<FileSystemModel *>(model());
    if (oldModel) {
        disconnect(oldModel, &FileSystemModel::rootPathChanged, this, nullptr);
    }
    if (newModel) {
        auto newFileSystemModel = qobject_cast<FileSystemModel *>(newModel);
        Q_ASSERT(newFileSystemModel);
        QTreeView::setModel(newModel);
        connect(oldModel, &FileSystemModel::rootPathChanged, [=](const QString &path) {
            setRootIndex(newFileSystemModel->index(path));
        });
    }
}
