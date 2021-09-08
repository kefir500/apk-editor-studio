#include "widgets/gradientwidget.h"
#include "base/utils.h"
#include <QPainter>
#include <QPaintEvent>

void GradientWidget::paintEvent(QPaintEvent *event)
{
    const int w = width();
    const int h = height();
    const int min = qMin(w, h);

    const QColor background = Utils::isLightTheme()
        ? QColor(250, 255, 230)
        : QPalette().color(QPalette::Window);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.fillRect(event->rect(), background);

    if (Utils::isLightTheme()) {
        QLinearGradient gradient(QPoint(0, min), QPoint(min, 0));
        gradient.setColorAt(0.1, background);
        gradient.setColorAt(0.5, QColor(240, 245, 220));
        gradient.setColorAt(1.0, background);
        painter.setBrush(gradient);
    } else {
        painter.setBrush(background.lighter(105));
    }

    QVector<QPoint> points;
    points.append(QPoint(0, min));
    points.append(QPoint(0, h));
    points.append(QPoint(w, h));
    points.append(QPoint(w, 0));
    points.append(QPoint(min, 0));

    painter.translate((w / 2.0 - min / 2.0), (h / 2.0 - min / 2.0));
    painter.drawPolygon(QPolygon(points));
}
