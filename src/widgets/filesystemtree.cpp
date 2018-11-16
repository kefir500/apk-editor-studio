#include "widgets/filesystemtree.h"
#include "apk/resourcemodelindex.h"

FilesystemTree::FilesystemTree(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &FilesystemTree::activated, [this](const QModelIndex &index) {
        emit editRequested(index);
    });
    connect(this, &FilesystemTree::customContextMenuRequested, [=](const QPoint &point) {
        const QModelIndex index = indexAt(point);
        if (index.isValid() && !model()->hasChildren(index)) {
            const QString path = ResourceModelIndex(index).path();
            if (!path.isEmpty()) {
                auto menu = generateContextMenu(index, path, this);
                if (menu) {
                    menu->exec(viewport()->mapToGlobal(point));
                }
            }
        }
    });
}

QFileSystemModel *FilesystemTree::model()
{
    return static_cast<QFileSystemModel *>(QTreeView::model());
}

void FilesystemTree::setModel(QFileSystemModel *model)
{
    QTreeView::setModel(model);
}
