#ifndef BASEACTIONSHEET_H
#define BASEACTIONSHEET_H

#include "sheets/basesheet.h"

class ElidedLabel;
class GradientWidget;
class QPushButton;
class QVBoxLayout;

class BaseActionSheet : public BaseSheet
{
    Q_OBJECT

public:
    explicit BaseActionSheet(QWidget *parent = nullptr);
    void setTitle(const QString &title);

protected:
    void addWidget(QWidget *widget);
    QPushButton *addButton(const QString &title = QString());
    void resizeEvent(QResizeEvent *event) override;

private:
    QVBoxLayout *layout;
    GradientWidget *background;
    ElidedLabel *label;
};

#endif // BASEACTIONSHEET_H
