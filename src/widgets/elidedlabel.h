#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>

class ElidedLabel : public QLabel
{
public:
    explicit ElidedLabel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ELIDEDLABEL_H
