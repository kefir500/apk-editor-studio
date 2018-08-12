#ifndef WELCOMETAB_H
#define WELCOMETAB_H

#include "editors/quicktab.h"

class WelcomeTab : public QuickTab
{
    Q_OBJECT

public:
    explicit WelcomeTab(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QPushButton *btnOpen;
    QPushButton *btnInstall;
};

#endif // WELCOMETAB_H
