#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include "widgets/resourcesbaseview.h"
#include <QTreeView>

class FilesystemTree : public QTreeView, public ResourcesBaseView
{
    Q_OBJECT

public:
    explicit FilesystemTree(QWidget *parent = nullptr);

signals:
    void editRequested(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // FILESYSTEMTREE_H
