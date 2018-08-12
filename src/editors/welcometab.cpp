#include "editors/welcometab.h"
#include "windows/dialogs.h"
#include "base/application.h"

WelcomeTab::WelcomeTab(QWidget *parent) : QuickTab(parent)
{
    btnOpen = addButton();
    btnInstall = addButton();
    connect(btnOpen, &QPushButton::clicked, [this]() { Dialogs::openApk(this); });
    connect(btnInstall, &QPushButton::clicked, app, &Application::installExternalApk);
}

void WelcomeTab::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        setTitle(tr("Welcome to the %1.").arg(app->getTitle()));
        btnOpen->setText(tr("Open APK"));
        btnInstall->setText(tr("Install APK"));
    } else {
        QWidget::changeEvent(event);
    }
}
