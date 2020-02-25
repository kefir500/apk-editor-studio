#include "editors/welcomeactionviewer.h"
#include "windows/dialogs.h"
#include "base/application.h"

WelcomeActionViewer::WelcomeActionViewer(QWidget *parent) : ActionViewer(parent)
{
    btnOpen = addButton();
    btnInstall = addButton();
    connect(btnOpen, &QPushButton::clicked, [=]() {
        app->actions.openApk(parent);
    });
    connect(btnInstall, &QPushButton::clicked, [=]() {
        app->actions.installExternalApks({}, {}, parent);
    });
}

void WelcomeActionViewer::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        //: Don't translate the "APK Editor Studio" part.
        setTitle(tr("Welcome to the APK Editor Studio."));
        btnOpen->setText(tr("Open APK"));
        btnInstall->setText(tr("Install APK"));
    }
    ActionViewer::changeEvent(event);
}
