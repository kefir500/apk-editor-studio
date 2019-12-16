#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>
#include <QTimer>

class LoadingWidget : public QWidget
{
public:
    explicit LoadingWidget(int size, QWidget *parent = nullptr);
    explicit LoadingWidget(QWidget *parent = nullptr): LoadingWidget(32, parent) {}

    void play();
    void stop();

protected:
    void paintEvent(QPaintEvent *event);

private:
    int spinnerSize;
    int spinnerAngle;
    QTimer spinnerTimer;
};

#endif // LOADINGWIDGET_H
