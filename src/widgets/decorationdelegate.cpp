#include "widgets/decorationdelegate.h"

DecorationDelegate::DecorationDelegate(const QSize &size, QObject *parent) : QStyledItemDelegate(parent)
{
    decorationSize = size;
}

void DecorationDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->decorationSize = decorationSize;
}
