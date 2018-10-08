#ifndef GRADIENTWIDGET_H
#define GRADIENTWIDGET_H

#include <QWidget>

class GradientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GradientWidget(QWidget *parent = nullptr) : QWidget(parent) {}

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

};

#endif // GRADIENTWIDGET_H
