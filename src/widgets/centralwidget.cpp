#include "widgets/centralwidget.h"
#include <QBoxLayout>

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent), widget(nullptr)
{
    setLayout(new QHBoxLayout);
    layout()->setMargin(0);
}

void CentralWidget::set(QWidget *newWidget)
{
    if (widget) {
        widget->hide();
        layout()->removeWidget(widget);
    }
    widget = newWidget;
    if (widget) {
        widget->show();
        layout()->addWidget(widget);
    }
}
