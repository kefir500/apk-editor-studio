#ifndef RESOURCEMODELINDEX_H
#define RESOURCEMODELINDEX_H

#include <QModelIndex>

class ResourceModelIndex : public QPersistentModelIndex
{
public:
    ResourceModelIndex() : QPersistentModelIndex() {}
    ResourceModelIndex(const QModelIndex &index) : QPersistentModelIndex(index) {}
    QString path() const;
    QIcon icon() const;
    void update();
};

#endif // RESOURCEMODELINDEX_H
