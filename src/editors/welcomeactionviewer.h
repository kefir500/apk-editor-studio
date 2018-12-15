#ifndef WELCOMEACTIONVIEWER_H
#define WELCOMEACTIONVIEWER_H

#include "editors/actionviewer.h"

class WelcomeActionViewer : public ActionViewer
{
    Q_OBJECT

public:
    explicit WelcomeActionViewer(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QPushButton *btnOpen;
    QPushButton *btnInstall;
};

#endif // WELCOMEACTIONVIEWER_H
