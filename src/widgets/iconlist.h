#ifndef ICONLIST_H
#define ICONLIST_H

#include "apk/iconitemsmodel.h"
#include <QListView>

class IconList : public QListView
{
public:
    explicit IconList(QWidget *parent = nullptr);

    IconItemsModel *model() const;
    void setModel(IconItemsModel *model);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void setHighlight(bool value);
};

#endif // ICONLIST_H
