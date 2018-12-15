#ifndef RESOURCEABSTRACTVIEW_H
#define RESOURCEABSTRACTVIEW_H

#include "apk/resourcemodelindex.h"
#include <QAbstractItemView>

class ResourceAbstractView : public QWidget
{
    Q_OBJECT

public:
    ResourceAbstractView(QAbstractItemView *view, QWidget *parent = nullptr);

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

#endif // RESOURCEABSTRACTVIEW_H
