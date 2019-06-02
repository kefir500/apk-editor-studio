#include "widgets/resourcetree.h"
#include "widgets/decorationsizedelegate.h"
#include <QSortFilterProxyModel>

ResourceTree::ResourceTree(QWidget *parent) : QTreeView(parent)
{
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegate(new DecorationSizeDelegate(QSize(16, 16), this));

    auto sortProxy = new QSortFilterProxyModel(this);
    sortProxy->setSortRole(ResourceItemsModel::SortRole);
    QTreeView::setModel(sortProxy);
}

ResourceItemsModel *ResourceTree::model() const
{
    auto proxy = static_cast<QSortFilterProxyModel *>(QTreeView::model());
    return static_cast<ResourceItemsModel *>(proxy->sourceModel());
}

void ResourceTree::setModel(QAbstractItemModel *model)
{
    if (model) {
        Q_ASSERT(qobject_cast<ResourceItemsModel *>(model));
        auto proxy = static_cast<QSortFilterProxyModel *>(QTreeView::model());
        proxy->setSourceModel(model);
        sortByColumn(0, Qt::AscendingOrder);
        setColumnWidth(ResourceItemsModel::NodeCaption, 120);
        setColumnWidth(ResourceItemsModel::ResourceLocale, 64);
        setColumnWidth(ResourceItemsModel::ResourcePath, 500);
    }
}
