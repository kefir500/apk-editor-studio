#include "widgets/filesystemtree.h"

QFileSystemModel *FilesystemTree::model() const
{
    return static_cast<QFileSystemModel *>(QTreeView::model());
}

void FilesystemTree::setModel(QAbstractItemModel *model)
{
    if (model) {
        Q_ASSERT(qobject_cast<QFileSystemModel *>(model));
        QTreeView::setModel(model);
    }
}
