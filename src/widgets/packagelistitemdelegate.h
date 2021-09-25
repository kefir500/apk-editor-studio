#ifndef PACKAGELISTITEMDELEGATE_H
#define PACKAGELISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class PackageListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PackageListItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // PACKAGELISTITEMDELEGATE_H
