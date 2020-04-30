#include "widgets/resourcetree.h"
#include "widgets/decorationsizedelegate.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

ResourceTree::ResourceTree(QWidget *parent) : QTreeView(parent)
{
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegate(new DecorationSizeDelegate(QSize(16, 16), this));

    sortProxy = new SortFilterProxyModel(this);
    sortProxy->setSortRole(ResourceItemsModel::SortRole);
    QTreeView::setModel(sortProxy);
}

ResourceItemsModel *ResourceTree::model() const
{
    return static_cast<ResourceItemsModel *>(sortProxy->sourceModel());
}

void ResourceTree::setModel(QAbstractItemModel *model)
{
    if (model) {
        Q_ASSERT(qobject_cast<ResourceItemsModel *>(model));
    }
    sortProxy->setSourceModel(model);
    sortByColumn(0, Qt::AscendingOrder);
    setColumnWidth(ResourceItemsModel::CaptionColumn, 120);
    setColumnWidth(ResourceItemsModel::LocaleColumn, 64);
    setColumnWidth(ResourceItemsModel::PathColumn, 500);
}
