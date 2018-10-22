#ifndef LOGDELEGATE_H
#define LOGDELEGATE_H

#include <QStyledItemDelegate>

class LogDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit LogDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QIcon iconExpand;
};

#endif // LOGDELEGATE_H
