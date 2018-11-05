#include "widgets/gradientwidget.h"
#include "base/application.h"
#include <QPainter>
#include <QPaintEvent>

void GradientWidget::paintEvent(QPaintEvent *event)
{
    const int w = width();
    const int h = height();
    const int min = qMin(w, h);

    const QColor color1(app->getColor(app->ColorBackgroundStart));
    const QColor color2(app->getColor(app->ColorBackgroundEnd));
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
