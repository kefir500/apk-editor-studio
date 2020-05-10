#include "widgets/filesystemtree.h"

FileSystemModel *FileSystemTree::model() const
{
    return static_cast<FileSystemModel *>(QTreeView::model());
}

void FileSystemTree::setModel(QAbstractItemModel *model)
{
    if (model) {
        Q_ASSERT(qobject_cast<FileSystemModel *>(model));
        QTreeView::setModel(model);
    }
}
