#ifndef LOGDELEGATE_H
#define LOGDELEGATE_H

#include <QStyledItemDelegate>

class LogDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit LogDelegate(QObject *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QIcon iconExpand;
};

#endif // LOGDELEGATE_H
