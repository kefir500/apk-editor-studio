#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QStyledItemDelegate>

class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ButtonDelegate(QObject *parent = nullptr);

    static QRect buttonRect(const QRect &rect);

signals:
    void clicked(int row) const;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    enum ButtonState {
        ButtonStateNone,
        ButtonStateHover,
        ButtonStatePressed
    };

//    QMap<int, ButtonState> buttons;
    int lastHoverRow;
};

#endif // BUTTONDELEGATE_H
