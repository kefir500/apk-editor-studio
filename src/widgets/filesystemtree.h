#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include "widgets/resourcesbaseview.h"
#include <QTreeView>
#include <QFileSystemModel>

class FilesystemTree : public QTreeView, public ResourcesBaseView
{
    Q_OBJECT

public:
    explicit FilesystemTree(QWidget *parent = nullptr);

    QFileSystemModel *model();
    void setModel(QFileSystemModel *model);

signals:
    void editRequested(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // FILESYSTEMTREE_H
