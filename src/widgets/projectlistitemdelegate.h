#ifndef PROJECTDELEGATE_H
#define PROJECTDELEGATE_H

#include <QStyledItemDelegate>

class ProjectListItemDelegate : public QStyledItemDelegate
{
public:
    explicit ProjectListItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QIcon iconReady;
    QIcon iconUnpacking;
    QIcon iconPacking;
    QIcon iconInstalling;
    QIcon iconError;
};

#endif // PROJECTDELEGATE_H
