#ifndef RESOURCEMODELINDEX_H
#define RESOURCEMODELINDEX_H

#include <QModelIndex>

class ResourceModelIndex : public QPersistentModelIndex
{
public:
    ResourceModelIndex(const QModelIndex &index) : QPersistentModelIndex(index) {}

    QString path() const;
    QIcon icon() const;

    bool save(QWidget *parent = nullptr) const;
    bool replace(QWidget *parent = nullptr);
    bool remove();
    bool explore() const;
    bool openWith(const QString &executable = {}) const;
    void update() const;
};

#endif // RESOURCEMODELINDEX_H
