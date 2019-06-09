#ifndef RESOURCEMODELINDEX_H
#define RESOURCEMODELINDEX_H

#include <QModelIndex>

class ResourceModelIndex : public QPersistentModelIndex
{
public:
    ResourceModelIndex(const QModelIndex &index) : QPersistentModelIndex(index) {}

    QString path() const;
    QIcon icon() const;

    void update();

private:
    QPair<QAbstractItemModel *, QModelIndex> map() const;
};

#endif // RESOURCEMODELINDEX_H
