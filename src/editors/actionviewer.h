#ifndef ACTIONVIEWER_H
#define ACTIONVIEWER_H

#include "editors/viewer.h"
#include "widgets/elidedlabel.h"
#include <QPushButton>

class QVBoxLayout;
class QPushButton;
class GradientWidget;

class ActionViewer : public Viewer
{
    Q_OBJECT

public:
    explicit ActionViewer(QWidget *parent = nullptr);
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

#endif // ACTIONVIEWER_H
