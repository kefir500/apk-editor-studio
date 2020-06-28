#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

class CentralWidget : public QWidget
{
public:
    explicit CentralWidget(QWidget *parent = nullptr);
    void set(QWidget *widget);

private:
    QWidget *widget;
};

#endif // CENTRALWIDGET_H
