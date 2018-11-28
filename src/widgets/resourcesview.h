#ifndef RESOURCESVIEW_H
#define RESOURCESVIEW_H

#include "apk/resourcemodelindex.h"
#include <QAbstractItemView>

class ResourcesView : public QWidget
{
    Q_OBJECT

public:
    ResourcesView(QAbstractItemView *view, QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model);

    template<class T>
    T getView() const {
        return dynamic_cast<T>(view);
    }

signals:
    void editRequested(const QModelIndex &index) const;

private:
    QSharedPointer<QMenu> generateContextMenu(const ResourceModelIndex &resourceIndex);

    QAbstractItemView *view;
};

#endif // RESOURCESVIEW_H
