#include "widgets/resourcestree.h"
#include "widgets/decorationdelegate.h"

ResourcesTree::ResourcesTree(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegate(new DecorationDelegate(QSize(16, 16), this));
    connect(this, &ResourcesTree::activated, [this](const QModelIndex &index) {
        emit editRequested(index);
    });
    connect(this, &ResourcesTree::customContextMenuRequested, [=](const QPoint &point) {
        const QModelIndex index = indexAt(point);
        if (!model()->hasChildren(index)) {
            const QString path = model()->getResourcePath(index);
            auto menu = generateContextMenu(index, path, this);
            if (menu) {
                menu->exec(viewport()->mapToGlobal(point));
            }
        }
    });
}

ResourcesModel *ResourcesTree::model()
{
    return static_cast<ResourcesModel *>(QTreeView::model());
}

void ResourcesTree::setModel(ResourcesModel *model)
{
    QTreeView::setModel(model);
}
