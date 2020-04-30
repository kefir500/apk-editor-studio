#include "widgets/deselectablelistview.h"
#include <QMouseEvent>

void DeselectableListView::mousePressEvent(QMouseEvent *event)
{
    QListView::mousePressEvent(event);
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {
        clearSelection();
        selectionModel()->setCurrentIndex({}, QItemSelectionModel::Select);
    }
}

void DeselectableListView::focusInEvent(QFocusEvent *event)
{
    const bool deselect = !currentIndex().isValid();
    QListView::focusInEvent(event);
    if (deselect) {
        selectionModel()->setCurrentIndex({}, QItemSelectionModel::Select);
        setAttribute(Qt::WA_InputMethodEnabled, false);
    }
}
