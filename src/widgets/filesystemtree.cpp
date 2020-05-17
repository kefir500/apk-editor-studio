#include "widgets/filesystemtree.h"

FileSystemModel *FileSystemTree::model() const
{
    return static_cast<FileSystemModel *>(QTreeView::model());
}

void FileSystemTree::setModel(QAbstractItemModel *newModel)
{
    const auto oldModel = model();
    if (oldModel) {
        disconnect(oldModel, &QFileSystemModel::rootPathChanged, this, nullptr);
    }
    if (newModel) {
        auto newFileSystemModel = qobject_cast<FileSystemModel *>(newModel);
        Q_ASSERT(newFileSystemModel);
        QTreeView::setModel(newFileSystemModel);
        connect(newFileSystemModel, &QFileSystemModel::rootPathChanged, [=](const QString &path) {
            setRootIndex(newFileSystemModel->index(path));
        });
    }
}
