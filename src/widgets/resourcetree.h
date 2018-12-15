#ifndef RESOURCETREE_H
#define RESOURCETREE_H

#include "apk/resourceitemsmodel.h"
#include <QTreeView>

class ResourceTree : public QTreeView
{
public:
    explicit ResourceTree(QWidget *parent = nullptr);

    ResourceItemsModel *model() const;
    void setModel(ResourceItemsModel *model);
};

#endif // RESOURCETREE_H
