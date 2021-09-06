#ifndef WELCOMESHEET_H
#define WELCOMESHEET_H

#include "sheets/baseactionsheet.h"

class MainWindow;

class WelcomeSheet : public BaseActionSheet
{
    Q_OBJECT

public:
    explicit WelcomeSheet(MainWindow *parent);

protected:
    void changeEvent(QEvent *event) override;

private:
    QPushButton *btnOpen;
    QPushButton *btnInstall;
    QPushButton *btnExplorer;
    QPushButton *btnDonate;
};

#endif // WELCOMESHEET_H
