#ifndef RESOURCESTREE_H
#define RESOURCESTREE_H

#include "widgets/resourcesbaseview.h"
#include "apk/resourcesmodel.h"
#include <QTreeView>

class ResourcesTree : public QTreeView, public ResourcesBaseView
{
    Q_OBJECT

public:
    explicit ResourcesTree(QWidget *parent = nullptr);

    ResourcesModel *model();
    void setModel(ResourcesModel *model);

signals:
    void editRequested(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // RESOURCESTREE_H
