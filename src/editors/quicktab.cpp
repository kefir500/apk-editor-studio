#include "editors/quicktab.h"
#include <QPainter>
#include <QPaintEvent>

QuickTab::QuickTab(QWidget *parent) : BaseEditor(QString(), QPixmap(), parent)
{
    label = new QLabel(this);
    label->setFont(QFont("Segoe UI", 14));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("margin-bottom: 6px;");

    layout = new QVBoxLayout(this);
    layout->setMargin(64);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();
}

void QuickTab::setTitle(const QString &title)
{
    label->setText(title);
}

bool QuickTab::load()
{
    return true;
}

bool QuickTab::save(const QString &as)
{
    Q_UNUSED(as)
    return true;
}

QPushButton *QuickTab::addButton(const QString &title)
{
    const QString btnStyle(
        "QPushButton { background: rgb(230, 240, 200); border: none; } "
        "QPushButton:hover { background: rgb(220, 230, 190); }"
        "QPushButton:pressed { background: rgb(210, 220, 180); }"
        //"QPushButton:focus { border: 2px solid rgb(200, 210, 170); }"
    );
    QPushButton *button = new QPushButton(this);
    button->setText(title);
    button->setMinimumHeight(34);
    button->setStyleSheet(btnStyle);
    layout->removeItem(layout->itemAt(layout->count() - 1));
    layout->addWidget(button);
    layout->addStretch();
    return button;
}

void QuickTab::paintEvent(QPaintEvent *event)
{
    QLinearGradient gradient1(event->rect().bottomLeft(), event->rect().topRight());
    gradient1.setColorAt(0, Qt::white);
    gradient1.setColorAt(1, QColor(220, 230, 190));

    QLinearGradient gradient2(event->rect().topLeft(), event->rect().bottomRight());
    gradient2.setColorAt(0, Qt::white);
    gradient2.setColorAt(1, QColor(245, 255, 225));

    const qreal ellipseCenterX = event->rect().right();
    const qreal ellipseCenterY = event->rect().top();
    const qreal ellipseRadiusX = event->rect().width() * 1.05;
    const qreal ellipseRadiusY = event->rect().height() / 1.3;

    QPainter painter(this);
    painter.fillRect(event->rect(), gradient1);
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient2);
    painter.drawEllipse(QPointF(ellipseCenterX, ellipseCenterY), ellipseRadiusX, ellipseRadiusY);
}
