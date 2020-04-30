#ifndef WELCOMEACTIONVIEWER_H
#define WELCOMEACTIONVIEWER_H

#include "editors/actionviewer.h"

class WelcomeActionViewer : public ActionViewer
{
    Q_OBJECT

public:
    explicit WelcomeActionViewer(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event) override;

private:
    QPushButton *btnOpen;
    QPushButton *btnInstall;
    QPushButton *btnExplorer;
};

#endif // WELCOMEACTIONVIEWER_H
