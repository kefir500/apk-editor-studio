#ifndef RESOURCESBASEVIEW_H
#define RESOURCESBASEVIEW_H

#include "widgets/resourcemenu.h"
#include <QModelIndex>

class ResourcesBaseView
{
public:
    virtual ~ResourcesBaseView() = default;

signals:
    virtual void editRequested(const QModelIndex &index) const = 0;

protected:
    QSharedPointer<ResourceMenu> generateContextMenu(const QPersistentModelIndex &resourceIndex, const QString &resourcePath, QWidget *parent = nullptr);
};

#endif // RESOURCESBASEVIEW_H
