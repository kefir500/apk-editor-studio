#include "widgets/decorationsizedelegate.h"

DecorationSizeDelegate::DecorationSizeDelegate(const QSize &size, QObject *parent) : QStyledItemDelegate(parent)
{
    decorationSize = size;
}

void DecorationSizeDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->decorationSize = decorationSize;
}
