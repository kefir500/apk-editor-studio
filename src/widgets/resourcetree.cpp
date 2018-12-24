#include "widgets/resourcetree.h"
#include "widgets/decorationsizedelegate.h"
#include "apk/resourcemodelindex.h"

ResourceTree::ResourceTree(QWidget *parent) : QTreeView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegate(new DecorationSizeDelegate(QSize(16, 16), this));
}

ResourceItemsModel *ResourceTree::model() const
{
    return static_cast<ResourceItemsModel *>(QTreeView::model());
}

void ResourceTree::setModel(ResourceItemsModel *model)
{
    QTreeView::setModel(model);
    sortByColumn(0, Qt::DescendingOrder);
    setColumnWidth(ResourceItemsModel::NodeCaption, 120);
    setColumnWidth(ResourceItemsModel::ResourceLocale, 64);
    setColumnWidth(ResourceItemsModel::ResourcePath, 500);
}
