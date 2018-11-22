#ifndef RESOURCESTREE_H
#define RESOURCESTREE_H

#include "apk/resourcesmodel.h"
#include <QTreeView>

class ResourcesTree : public QTreeView
{
public:
    explicit ResourcesTree(QWidget *parent = nullptr);

    ResourcesModel *model() const;
    void setModel(ResourcesModel *model);
};

#endif // RESOURCESTREE_H
