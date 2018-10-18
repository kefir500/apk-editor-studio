#include "editors/quicktab.h"
#include "base/application.h"
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
        "QPushButton { background: rgb(225, 240, 190); border: none; } "
        "QPushButton:hover { background: rgb(215, 230, 180); }"
        "QPushButton:pressed { background: rgb(205, 220, 170); }"
    );
    QPushButton *button = new QPushButton(this);
    button->setText(title);
    button->setMinimumHeight(app->scale(34));
    button->setStyleSheet(btnStyle);
    layout->removeItem(layout->itemAt(layout->count() - 1));
    layout->addWidget(button);
    layout->addStretch();
    return button;
}

void QuickTab::paintEvent(QPaintEvent *event)
{

    const int w = width();
    const int h = height();
    const QColor color1(app->getColor(app->ColorBackgroundStart));
    const QColor color2(app->getColor(app->ColorBackgroundEnd));

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.fillRect(event->rect(), color1);

    const QPoint points[] = {
        QPoint(0, h / 2 + 150),
        QPoint(0, h),
        QPoint(w, h),
        QPoint(w, h / 2 - 130),
    };

    painter.setBrush(color2);
    painter.drawPolygon(points, 4);
}
