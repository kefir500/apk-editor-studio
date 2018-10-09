#ifndef DECORATIONDELEGATE_H
#define DECORATIONDELEGATE_H

#include <QStyledItemDelegate>

class DecorationDelegate : public QStyledItemDelegate
{
public:
    explicit DecorationDelegate(const QSize &size, QObject *parent = nullptr);

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QSize decorationSize;
};

#endif // DECORATIONDELEGATE_H
