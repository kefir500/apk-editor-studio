#include "widgets/loadingwidget.h"
#include "base/application.h"
#include <QPainter>

LoadingWidget::LoadingWidget(int size, QWidget *parent) : QWidget(parent), spinnerSize(size)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    spinnerAngle = 0;
    connect(&spinnerTimer, &QTimer::timeout, [=]() {
        spinnerAngle = (spinnerAngle > 0) ? spinnerAngle - 80 : 5760;
        update();
    });
    spinnerTimer.setInterval(15);
    stop();
}

void LoadingWidget::play()
{
    show();
    spinnerTimer.start();
}

void LoadingWidget::stop()
{
    hide();
    spinnerTimer.stop();
}

void LoadingWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    const int w = 40;
    const int h = 40;
    const int x = rect().center().x() - w / 2;
    const int y = rect().center().y() - h / 2;

    QPainter painter(this);
    painter.setPen(QPen(QPalette().windowText(), app->scale(2.2)));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawArc(x, y, w, h, spinnerAngle, 12 * 360);
}
