#include "widgets/buttondelegate.h"
#include "base/application.h"
#include <QPainter>

ButtonDelegate::ButtonDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    lastHoverRow = -1;
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    const bool editable = index.data(ManifestModel::ReferenceRole).toBool();
    if (editable) {

        const int currentHoverRow = index.row();
        const ButtonState buttonState = static_cast<ButtonState>(index.data(Qt::UserRole).toInt());

        if (event->type() == QEvent::MouseMove) {
            // Set the button state to normal if cursor was moved to another row
            if (lastHoverRow != currentHoverRow) {
                model->setData(model->index(lastHoverRow, 0), ButtonStateNone, Qt::UserRole);
                lastHoverRow = currentHoverRow;
            }
            QMouseEvent *mouse = static_cast<QMouseEvent *>(event);
            if (buttonRect(option.rect).contains(mouse->pos())) {
                // Set the button state to hover if cursor is over the button
                if (buttonState == ButtonStatePressed)  {
                    return true;
                }
                model->setData(index, ButtonStateHover, Qt::UserRole);
            } else {
                // Set the button state to normal if cursor is not over the button
                model->setData(index, ButtonStateNone, Qt::UserRole);
                return true;
            }

        } else if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *click = static_cast<QMouseEvent *>(event);
            if (click->button() == Qt::LeftButton) {
                if (buttonRect(option.rect).contains(click->pos())) {
                    model->setData(index, ButtonStatePressed, Qt::UserRole);
                    return true;
                }
            }

        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouse = static_cast<QMouseEvent *>(event);
            if (buttonRect(option.rect).contains(mouse->pos())) {
                model->setData(index, ButtonStateHover, Qt::UserRole);
                if (buttonState == ButtonStatePressed) {
                    emit clicked(index.row());
                }
                return true;
            }
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const bool editable = index.data(ManifestModel::ReferenceRole).toBool();
    if (editable) {
        // Prepare button:
        QStyle::StateFlag buttonState;
        switch (index.data(Qt::UserRole).toInt()) {
        case ButtonStateNone:
            buttonState = QStyle::State_None;
            break;
        case ButtonStatePressed:
            buttonState = QStyle::State_Sunken;
            break;
        case ButtonStateHover:
            if (option.state & QStyle::State_MouseOver) {
                buttonState = QStyle::State_MouseOver;
                break;
            }
        default:
            buttonState = QStyle::State_None;
        }
        QStyleOptionButton button;
        button.text = "@";
        button.rect = buttonRect(option.rect);
        button.state = QStyle::State_Enabled | QStyle::State_Raised | buttonState;
        // Paint base:
        QStyleOptionViewItem base = option;
        base.rect.setWidth(base.rect.width() - button.rect.width());
        QStyledItemDelegate::paint(painter, base, index);
        // Paint button:
        app->style()->drawControl(QStyle::CE_PushButton, &button, painter);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QRect ButtonDelegate::buttonRect(const QRect &rect)
{
    const int w = rect.height();
    const int h = rect.height();
    const int x = rect.left() + rect.width() - w;
    const int y = rect.top();
    return QRect(x, y, w, h);
}
