#ifndef RESOURCETREE_H
#define RESOURCETREE_H

#include "apk/resourceitemsmodel.h"
#include "apk/sortfilterproxymodel.h"
#include <QTreeView>

class ResourceTree : public QTreeView
{
    Q_OBJECT

public:
    explicit ResourceTree(QWidget *parent = nullptr);

    ResourceItemsModel *model() const;
    void setModel(QAbstractItemModel *model) override;

private:
    SortFilterProxyModel *sortProxy;
};

#endif // RESOURCETREE_H
