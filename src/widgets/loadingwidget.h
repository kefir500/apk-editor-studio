#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QWidget>
#include <QTimer>

class LoadingWidget : public QWidget
{
public:
    explicit LoadingWidget(int size, QWidget *parent = nullptr);
    explicit LoadingWidget(QWidget *parent = nullptr) : LoadingWidget(32, parent) {}

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
    bool event(QEvent *event) override;

private:
    void parentChanged();

    int spinnerSize;
    int spinnerAngle;
    QTimer spinnerTimer;
};

#endif // LOADINGWIDGET_H
