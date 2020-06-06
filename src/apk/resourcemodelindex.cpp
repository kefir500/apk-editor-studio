#include "apk/resourcemodelindex.h"
#include "apk/iresourceitemsmodel.h"
#include "base/utils.h"
#include <QIcon>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

QString ResourceModelIndex::path() const
{
    return qobject_cast<IResourceItemsModel *>(model())->getResourcePath(*this);
}

QIcon ResourceModelIndex::icon() const
{
    return data(Qt::DecorationRole).value<QIcon>();
}

bool ResourceModelIndex::save(QWidget *parent) const
{
    return Utils::copyFile(path(), parent);
}

bool ResourceModelIndex::replace(QWidget *parent)
{
    return qobject_cast<IResourceItemsModel *>(model())->replaceResource(*this, {}, parent);
}

bool ResourceModelIndex::remove()
{
    return qobject_cast<IResourceItemsModel *>(model())->removeResource(*this);
}

bool ResourceModelIndex::explore() const
{
    return Utils::explore(path());
}

void ResourceModelIndex::update() const
{
    emit const_cast<QAbstractItemModel *>(model())->dataChanged(*this, *this);
}
