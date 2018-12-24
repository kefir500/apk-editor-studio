#include "widgets/filesystemtree.h"
#include "apk/resourcemodelindex.h"

QFileSystemModel *FilesystemTree::model() const
{
    return static_cast<QFileSystemModel *>(QTreeView::model());
}

void FilesystemTree::setModel(QFileSystemModel *model)
{
    QTreeView::setModel(model);
}
