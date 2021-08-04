#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = nullptr);
    void set(QWidget *widget);

private:
    QWidget *widget;
};

#endif // CENTRALWIDGET_H
