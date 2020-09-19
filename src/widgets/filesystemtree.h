#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include "apk/filesystemmodel.h"
#include <QTreeView>

class FileSystemTree : public QTreeView
{
public:
    explicit FileSystemTree(QWidget *parent = nullptr);
    FileSystemModel *model() const;
    void setModel(QAbstractItemModel *model) override;
};

#endif // FILESYSTEMTREE_H
