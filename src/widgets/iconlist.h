#ifndef ICONLIST_H
#define ICONLIST_H

#include "apk/iconitemsmodel.h"
#include <QTreeView>

class IconList : public QTreeView
{
public:
    explicit IconList(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model) override;
    IconItemsModel *model() const;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void expandApplicationIcons();

    QRubberBand *rubberBand;
};

#endif // ICONLIST_H
