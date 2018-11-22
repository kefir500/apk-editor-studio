#include "widgets/filesystemtree.h"
#include "apk/resourcemodelindex.h"

FilesystemTree::FilesystemTree(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
}

QFileSystemModel *FilesystemTree::model() const
{
    return static_cast<QFileSystemModel *>(QTreeView::model());
}

void FilesystemTree::setModel(QFileSystemModel *model)
{
    QTreeView::setModel(model);
}
