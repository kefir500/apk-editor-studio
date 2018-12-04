#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>

class ElidedLabel : public QLabel
{
public:
    explicit ElidedLabel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

#endif // ELIDEDLABEL_H
