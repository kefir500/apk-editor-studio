#ifndef IRESOURCEITEMSMODEL_H
#define IRESOURCEITEMSMODEL_H

#include <QModelIndex>
#include "base/utils.h"

class IResourceItemsModel
{
public:
    enum Role {
        PathRole = Qt::UserRole + 1,
        IconRole,
        RoleCount
    };
    virtual ~IResourceItemsModel() = default;
    bool saveResource(const QModelIndex &index, const QString &path = QString());
    virtual bool replaceResource(const QModelIndex &index, const QString &path = QString()) = 0;
    virtual bool removeResource(const QModelIndex &index) = 0;
};

Q_DECLARE_INTERFACE(IResourceItemsModel, "org.qwertycube.IResourceItemsModel")

#endif // IRESOURCEITEMSMODEL_H
