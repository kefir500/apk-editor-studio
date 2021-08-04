#ifndef LOGDELEGATE_H
#define LOGDELEGATE_H

#include <QStyledItemDelegate>
#include <QTimer>

class LogDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit LogDelegate(QObject *parent = nullptr);
    void setLoading(bool loading);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QIcon iconExpand;
    QTimer spinnerTimer;
    int spinnerAngle;

signals:
    void updated();
};

#endif // LOGDELEGATE_H
