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
    QTreeView::setModel(newModel);
    if (newModel) {
        auto newFileSystemModel = qobject_cast<FileSystemModel *>(newModel);
        Q_ASSERT(newFileSystemModel);
        setRootIndex(newFileSystemModel->rootIndex());
        connect(newFileSystemModel, &QFileSystemModel::rootPathChanged, this, [=](const QString &path) {
            setRootIndex(newFileSystemModel->index(path));
        });
    }
}
