#include "widgets/elidedlabel.h"
#include <QPainter>

ElidedLabel::ElidedLabel(QWidget *parent) : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
}

void ElidedLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QFontMetrics fontMetrics = this->fontMetrics();
    if (fontMetrics.horizontalAdvance(text()) > contentsRect().width()) {
        QString elidedText = fontMetrics.elidedText(text().trimmed(), Qt::ElideRight, width());
        painter.drawText(rect(), elidedText);
    } else {
        QLabel::paintEvent(event);
    }
}
