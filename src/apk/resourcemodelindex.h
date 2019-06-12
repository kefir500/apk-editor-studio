#ifndef RESOURCEMODELINDEX_H
#define RESOURCEMODELINDEX_H

#include <QModelIndex>

class ResourceModelIndex : public QPersistentModelIndex
{
public:
    ResourceModelIndex(const QModelIndex &index) : QPersistentModelIndex(index) {}

    QString path() const;
    QIcon icon() const;

    bool save() const;
    bool replace();
    bool remove();
    bool explore() const;
    void update() const;
};

#endif // RESOURCEMODELINDEX_H
