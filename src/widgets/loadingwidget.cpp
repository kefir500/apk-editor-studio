#include "widgets/loadingwidget.h"
#include "base/utils.h"
#include <QResizeEvent>
#include <QPainter>
#include <chrono>

using namespace std::chrono_literals;

LoadingWidget::LoadingWidget(int size, QWidget *parent) : QWidget(parent), spinnerSize(size)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    parentChanged();

    connect(&spinnerTimer, &QTimer::timeout, this, [this]() {
        spinnerAngle = (spinnerAngle > 0) ? spinnerAngle - 80 : 5760;
        update();
    });
    spinnerTimer.setInterval(15ms);
}

void LoadingWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    const int w = spinnerSize;
    const int h = spinnerSize;
    const int x = rect().center().x() - w / 2;
    const int y = rect().center().y() - h / 2;

    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Window));
    painter.setPen(QPen(palette().color(QPalette::WindowText), Utils::scale(2.2)));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawArc(x, y, w, h, spinnerAngle, 12 * 360);
}

void LoadingWidget::showEvent(QShowEvent *)
{
    if (parent()) {
        qobject_cast<QWidget *>(parent())->setEnabled(false);
    }
    spinnerTimer.start();
}

void LoadingWidget::hideEvent(QHideEvent *)
{
    if (parent()) {
        qobject_cast<QWidget *>(parent())->setEnabled(true);
    }
    spinnerTimer.stop();
}

bool LoadingWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == parent()) {
        if (event->type() == QEvent::Resize) {
            resize(static_cast<QResizeEvent *>(event)->size());
        } else if (event->type() == QEvent::ChildAdded) {
            raise();
        }
    }
    return QWidget::eventFilter(object, event);
}

bool LoadingWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ParentAboutToChange) {
        if (parent()) {
            parent()->removeEventFilter(this);
        }
    } else if (event->type() == QEvent::ParentChange) {
        parentChanged();
    }
    return QWidget::event(event);
}

void LoadingWidget::parentChanged()
{
    if (parent()) {
        parent()->installEventFilter(this);
        raise();
    }
}
