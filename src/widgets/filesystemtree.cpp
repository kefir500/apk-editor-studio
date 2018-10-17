#include "widgets/filesystemtree.h"
#include <QFileSystemModel>

FilesystemTree::FilesystemTree(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &FilesystemTree::activated, [this](const QModelIndex &index) {
        emit editRequested(index);
    });
    connect(this, &FilesystemTree::customContextMenuRequested, [=](const QPoint &point) {
        const QModelIndex index = indexAt(point);
        if (!model()->hasChildren(index)) {
            const QString path = static_cast<QFileSystemModel *>(model())->filePath(index);
            auto menu = generateContextMenu(index, path, this);
            if (menu) {
                menu->exec(viewport()->mapToGlobal(point));
            }
        }
    });
}
