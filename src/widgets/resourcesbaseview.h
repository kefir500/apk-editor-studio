#ifndef RESOURCESBASEVIEW_H
#define RESOURCESBASEVIEW_H

#include "widgets/resourcemenu.h"

class ResourcesBaseView
{
public:
    virtual ~ResourcesBaseView() {}

signals:
    virtual void editRequested(const QModelIndex &index) const = 0;

protected:
    QSharedPointer<ResourceMenu> generateContextMenu(const QPersistentModelIndex &resourceIndex, const QString &resourcePath, QWidget *parent = nullptr);
};

#endif // RESOURCESBASEVIEW_H
