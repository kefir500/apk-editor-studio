#include "widgets/logdelegate.h"
#include "base/application.h"
#include <QPainter>

LogDelegate::LogDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    iconExpand = app->icons.get("more.png");
}

void LogDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    const QString message = index.sibling(index.row(), LogModel::LogDescriptive).data().toString();
    if (!message.isEmpty()) {
        const int h = option.rect.height() - 8;
        const int w = h;
        const int x = option.rect.right() - w - 2;
        const int y = option.rect.center().y() - h / 2;
        iconExpand.paint(painter, x, y, w, h);
    }
}
