#ifndef FILESYSTEMTREE_H
#define FILESYSTEMTREE_H

#include <QTreeView>

class FileSystemModel;

class FileSystemTree : public QTreeView
{
    Q_OBJECT

public:
    explicit FileSystemTree(QWidget *parent = nullptr);
    FileSystemModel *model() const;
    void setModel(QAbstractItemModel *model) override;
};

#endif // FILESYSTEMTREE_H
