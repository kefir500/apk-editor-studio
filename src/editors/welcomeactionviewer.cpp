#include "editors/welcomeactionviewer.h"
#include "windows/dialogs.h"
#include "base/application.h"

WelcomeActionViewer::WelcomeActionViewer(MainWindow *parent) : ActionViewer(parent)
{
    btnOpen = addButton();
    btnInstall = addButton();
    btnExplorer = addButton();
    connect(btnOpen, &QPushButton::clicked, parent, [parent]() {
        app->actions.openApk(parent);
    });
    connect(btnInstall, &QPushButton::clicked, parent, [parent]() {
        app->actions.installApk(parent);
    });
    connect(btnExplorer, &QPushButton::clicked, parent, [parent]() {
        app->actions.openAndroidExplorer(parent);
    });
}

void WelcomeActionViewer::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        //: Don't translate the "APK Editor Studio" part.
        setTitle(tr("Welcome to the APK Editor Studio."));
        btnOpen->setText(tr("Open APK"));
        btnInstall->setText(tr("Install APK"));
        btnExplorer->setText(app->translate("AndroidExplorer", "Android Explorer"));
    }
    ActionViewer::changeEvent(event);
}
