#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include "apk/resourceitemsmodel.h"
#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel, public IResourceItemsModel
{
    Q_OBJECT
    Q_INTERFACES(IResourceItemsModel)

public:
    SortFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

    ResourceItemsModel *sourceModel() const;
    bool replaceResource(const QModelIndex &index, const QString &path = QString()) override;
    bool removeResource(const QModelIndex &index) override;
    QString getResourcePath(const QModelIndex &index) const override;
};

#endif // SORTFILTERPROXYMODEL_H
