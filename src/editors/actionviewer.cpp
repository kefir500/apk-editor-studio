#include "editors/actionviewer.h"
#include "base/application.h"
#include "base/utils.h"
#include <QPainter>
#include <QPaintEvent>
#include <QFontDatabase>

ActionViewer::ActionViewer(QWidget *parent) : Viewer(parent)
{
    label = new ElidedLabel(this);
#if defined(Q_OS_WIN)
    label->setFont(QFont("Segoe UI", 14));
#elif defined(Q_OS_OSX)
    label->setFont(QFont(".SF NS Text", 18));
#else
    QFont font = QFontDatabase::systemFont(QFontDatabase::TitleFont);
    font.setPointSize(14);
    label->setFont(font);
#endif
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("margin-bottom: 6px;");

    layout = new QVBoxLayout(this);
    layout->setMargin(64);
    layout->setSpacing(6);
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();
}

void ActionViewer::setTitle(const QString &title)
{
    label->setText(title);
}

void ActionViewer::addWidget(QWidget *widget)
{
    layout->insertWidget(layout->count() - 1, widget);
}

QPushButton *ActionViewer::addButton(const QString &title)
{
    QPushButton *button = new QPushButton(this);
    button->setText(title);
    button->setMinimumHeight(app->scale(34));
    if (!Utils::isDarkTheme()) {
        const QString btnStyle(
            "QPushButton { background: rgb(225, 240, 190); border: none; padding: 0 20px } "
            "QPushButton:hover { background: rgb(215, 230, 180); }"
            "QPushButton:pressed { background: rgb(205, 220, 170); }"
        );
        button->setStyleSheet(btnStyle);
    }
    addWidget(button);
    return button;
}

void ActionViewer::paintEvent(QPaintEvent *event)
{
    const int w = width();
    const int h = height();
    const int min = qMin(w, h);

    const QColor color1(app->theme()->color(Theme::Color::BackgroundGradientStart));
    const QColor color2(app->theme()->color(Theme::Color::BackgroundGradientEnd));
    QLinearGradient gradient(QPoint(0, min), QPoint(min, 0));
    gradient.setColorAt(0.1, color1);
    gradient.setColorAt(0.5, color2);
    gradient.setColorAt(1.0, color1);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.fillRect(event->rect(), color1);

    QVector<QPoint> points;
    points.append(QPoint(0, min));
    points.append(QPoint(0, h));
    points.append(QPoint(w, h));
    points.append(QPoint(w, 0));
    points.append(QPoint(min, 0));

    painter.translate((w / 2.0 - min / 2.0), (h / 2.0 - min / 2.0));
    painter.setBrush(gradient);
    painter.drawPolygon(QPolygon(points));
}
