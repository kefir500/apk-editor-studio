#include "widgets/poollistwidget.h"
#include <QDropEvent>

PoolListWidget::PoolListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void PoolListWidget::addItem(QListWidgetItem *item, bool reusable)
{
    item->setData(ReusableRole, reusable);
    QListWidget::addItem(item);
}

void PoolListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();
    if (item && item->data(ReusableRole).toBool()) {
        supportedActions = Qt::CopyAction;
    }
    QListWidget::startDrag(supportedActions);
}

void PoolListWidget::dropEvent(QDropEvent *event)
{
    QListWidget *source = static_cast<QListWidget *>(event->source());
    if (source != this) {
        QListWidgetItem *item = source->currentItem();
        if (item && item->data(ReusableRole).toBool()) {
            // TODO Get rid of black stripe when dropping item
            delete item;
        } else {
            QListWidget::dropEvent(event);
        }
    }
}
