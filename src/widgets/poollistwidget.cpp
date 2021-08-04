#include "widgets/poollistwidget.h"
#include <QDropEvent>

PoolListWidget::PoolListWidget(QWidget *parent) : QListWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    connect(this, &QListWidget::doubleClicked, this, [this](const QModelIndex &index) {
        QListWidgetItem *targetItem = item(index.row());
        emit pulled(targetItem);
        if (!isReusable(targetItem)) {
            delete targetItem;
        }
    });
}

void PoolListWidget::addItem(QListWidgetItem *item, bool reusable)
{
    item->setData(ReusableRole, reusable);
    QListWidget::addItem(item);
}

void PoolListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();
    if (item && isReusable(item)) {
        supportedActions = Qt::CopyAction;
    }
    QListWidget::startDrag(supportedActions);
}

void PoolListWidget::dropEvent(QDropEvent *event)
{
    QListWidget *source = static_cast<QListWidget *>(event->source());
    if (source != this) {
        QListWidgetItem *item = source->currentItem();
        if (item && isReusable(item)) {
            delete item;
        } else {
            QListWidget::dropEvent(event);
        }
    }
}

bool PoolListWidget::isReusable(QListWidgetItem *item)
{
    return item->data(PoolListWidget::ReusableRole).toBool();
}
