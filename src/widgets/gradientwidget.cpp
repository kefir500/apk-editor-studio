#include "widgets/gradientwidget.h"
#include <QPainter>
#include <QPaintEvent>

void GradientWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QLinearGradient gradient1(0, 0, width(), height());
    gradient1.setColorAt(0, Qt::white);
    gradient1.setColorAt(1, QColor(220, 230, 190));

    QLinearGradient gradient2(0, 0, width(), height());
    gradient2.setColorAt(0, Qt::white);
    gradient2.setColorAt(1, QColor(245, 255, 225));

    const qreal ellipseCenterX = rect().right();
    const qreal ellipseCenterY = rect().top();
    const qreal ellipseRadiusX = rect().width() * 1.05;
    const qreal ellipseRadiusY = rect().height() / 1.3;

    QPainter painter(this);
    painter.fillRect(rect(), gradient1);
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient2);
    painter.drawEllipse(QPointF(ellipseCenterX, ellipseCenterY), ellipseRadiusX, ellipseRadiusY);
}
