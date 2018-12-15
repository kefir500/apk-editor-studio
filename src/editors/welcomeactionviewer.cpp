#include "editors/welcomeactionviewer.h"
#include "windows/dialogs.h"
#include "base/application.h"

WelcomeActionViewer::WelcomeActionViewer(QWidget *parent) : ActionViewer(parent)
{
    btnOpen = addButton();
    btnInstall = addButton();
    connect(btnOpen, &QPushButton::clicked, [this]() { Dialogs::openApk(this); });
    connect(btnInstall, &QPushButton::clicked, app, &Application::installExternalApk);
}

void WelcomeActionViewer::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        //: Don't translate the "APK Editor Studio" part.
        setTitle(tr("Welcome to the APK Editor Studio."));
        btnOpen->setText(tr("Open APK"));
        btnInstall->setText(tr("Install APK"));
    } else {
        QWidget::changeEvent(event);
    }
}
