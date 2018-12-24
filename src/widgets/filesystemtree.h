#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include <QFileSystemModel>
#include <QTreeView>

class FilesystemTree : public QTreeView
{
public:
    explicit FilesystemTree(QWidget *parent = nullptr) : QTreeView(parent) {}

    QFileSystemModel *model() const;
    void setModel(QFileSystemModel *model);
};

#endif // FILESYSTEMTREE_H
