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

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.fillRect(event->rect(), color1);

    const QPoint points[] = {
        QPoint(0, min),
        QPoint(0, h),
        QPoint(w, h),
        QPoint(w, 0),
        QPoint(min, 0)
    };
    painter.translate(w / 2.0 - min / 2.0, h / 2.0 - min / 2.0);
    painter.setBrush(color2);
    painter.drawPolygon(points, 5);
}
