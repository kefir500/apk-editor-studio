#ifndef PROJECTDELEGATE_H
#define PROJECTDELEGATE_H

#include <QStyledItemDelegate>

class ProjectDelegate : public QStyledItemDelegate {

public:
    explicit ProjectDelegate(QObject *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QPixmap iconReady;
    QPixmap iconUnpacking;
    QPixmap iconPacking;
    QPixmap iconInstalling;
    QPixmap iconError;
};

#endif // PROJECTDELEGATE_H
