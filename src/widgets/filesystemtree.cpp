#include "widgets/filesystemtree.h"

FileSystemModel *FilesystemTree::model() const
{
    return static_cast<FileSystemModel *>(QTreeView::model());
}

void FilesystemTree::setModel(QAbstractItemModel *model)
{
    if (model) {
        Q_ASSERT(qobject_cast<FileSystemModel *>(model));
        QTreeView::setModel(model);
    }
}
