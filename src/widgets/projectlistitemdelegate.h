#ifndef PROJECTDELEGATE_H
#define PROJECTDELEGATE_H

#include <QStyledItemDelegate>

class ProjectListItemDelegate : public QStyledItemDelegate
{
public:
    explicit ProjectListItemDelegate(QObject *parent = nullptr) {}

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // PROJECTDELEGATE_H
