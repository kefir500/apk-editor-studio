#ifndef RESOURCEVIEWCONTAINER_H
#define RESOURCEVIEWCONTAINER_H

#include <QAbstractItemView>

class ResourceViewContainer : public QWidget
{
    Q_OBJECT

public:
    explicit ResourceViewContainer(QAbstractItemView *view, QWidget *parent = 0);
    QAbstractItemModel *model() const;

signals:
    void requestForEdit(const QModelIndex &index) const;

private:
    void openContextMenu(const QPoint &point, const QModelIndex &index);
    QString getResourcePath(const QModelIndex &index) const;

    QAbstractItemView *view;
};

#endif // RESOURCEVIEWCONTAINER_H
