#ifndef WELCOMEVIEWER_H
#define WELCOMEVIEWER_H

#include "editors/actionviewer.h"

class WelcomeViewer : public ActionViewer
{
    Q_OBJECT

public:
    explicit WelcomeViewer(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QPushButton *btnOpen;
    QPushButton *btnInstall;
};

#endif // WELCOMEVIEWER_H
