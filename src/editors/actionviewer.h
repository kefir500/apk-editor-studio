#ifndef ACTIONVIEWER_H
#define ACTIONVIEWER_H

#include "editors/viewer.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <functional>

class ActionViewer : public Viewer
{
    Q_OBJECT

public:
    explicit ActionViewer(QWidget *parent = nullptr);
    void setTitle(const QString &title);

protected:
    QPushButton *addButton(const QString &title = QString());
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QVBoxLayout *layout;
    QLabel *label;
};

#endif // ACTIONVIEWER_H
