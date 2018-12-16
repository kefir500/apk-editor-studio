#ifndef ITEMBUTTONDELEGATE_H
#define ITEMBUTTONDELEGATE_H

#include <QStyledItemDelegate>

class ItemButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ItemButtonDelegate(QObject *parent = nullptr);

    static QRect buttonRect(const QRect &rect);

signals:
    void clicked(int row) const;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    enum ButtonState {
        ButtonStateNone,
        ButtonStateHover,
        ButtonStatePressed
    };

    int lastHoverRow;
};

#endif // ITEMBUTTONDELEGATE_H
