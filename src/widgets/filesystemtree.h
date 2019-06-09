#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include "apk/filesystemmodel.h"
#include <QTreeView>

class FilesystemTree : public QTreeView
{
public:
    explicit FilesystemTree(QWidget *parent = nullptr) : QTreeView(parent) {}
    FileSystemModel *model() const;
    void setModel(QAbstractItemModel *model) override;
};

#endif // FILESYSTEMTREE_H
