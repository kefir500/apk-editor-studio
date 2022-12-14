#ifndef RESOURCEABSTRACTVIEW_H
#define RESOURCEABSTRACTVIEW_H

#include <QMenu>

class QAbstractItemModel;
class QAbstractItemView;
class ResourceModelIndex;

class ResourceAbstractView : public QWidget
{
    Q_OBJECT

public:
    ResourceAbstractView(QAbstractItemView *view, QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model);

    template <class T>
    T getView() const {
        return dynamic_cast<T>(view);
    }

signals:
    void editRequested(const QModelIndex &index);

private:
    QSharedPointer<QMenu> generateContextMenu(ResourceModelIndex &resourceIndex);
    QMenu *generateOpenWithMenu(ResourceModelIndex &resourceIndex, QWidget *parent = nullptr);

    QAbstractItemView *view;
};

#endif // RESOURCEABSTRACTVIEW_H
