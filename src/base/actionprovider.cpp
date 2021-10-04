#include "base/actionprovider.h"
#include "base/application.h"
#include "base/settings.h"
#include "base/updater.h"
#include "base/utils.h"
#include "windows/androidexplorer.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "windows/keymanager.h"
#include "windows/optionsdialog.h"
#include "windows/rememberdialog.h"
#include "tools/adb.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QMenu>
#include <QUrl>

void ActionProvider::visitWebPage() const
{
    QDesktopServices::openUrl(Utils::getWebsiteUtmUrl());
}

void ActionProvider::visitSourcePage() const
{
    QDesktopServices::openUrl(Utils::getRepositoryUrl());
}

void ActionProvider::visitDonatePage() const
{
    QDesktopServices::openUrl(Utils::getDonationsUrl());
}

void ActionProvider::visitUpdatePage() const
{
    QDesktopServices::openUrl(Utils::getUpdateUrl());
}

void ActionProvider::visitBlogPage(const QString &post) const
{
    QDesktopServices::openUrl(Utils::getBlogPostUrl(post));
}

void ActionProvider::checkUpdates(QWidget *parent) const
{
    Updater::check(true, parent);
}

bool ActionProvider::resetSettings(QWidget *parent) const
{
    const QString question(tr("Are you sure you want to reset settings?"));
    const int answer = QMessageBox::question(parent, {}, question);
    if (answer != QMessageBox::Yes) {
        return false;
    }
    app->settings->reset();
    return true;
}

void ActionProvider::openOptions(QWidget *parent) const
{
    OptionsDialog settings(parent);
    settings.exec();
}

void ActionProvider::openDeviceManager(QWidget *parent) const
{
    DeviceManager deviceManager(parent);
    deviceManager.exec();
}

void ActionProvider::openKeyManager(QWidget *parent) const
{
    KeyManager keyManager(parent);
    keyManager.exec();
}

void ActionProvider::openAndroidExplorer(QWidget *parent) const
{
    const auto device = Dialogs::getExplorerDevice(parent);
    if (device.isNull()) {
        return;
    }
    auto explorer = new AndroidExplorer(device.getSerial());
    explorer->setAttribute(Qt::WA_DeleteOnClose);
    explorer->show();
}

void ActionProvider::takeScreenshot(QWidget *parent) const
{
    const auto device = Dialogs::getScreenshotDevice(parent);
    if (!device.isNull()) {
        takeScreenshot(device.getSerial(), parent);
    }
}

void ActionProvider::takeScreenshot(const QString &serial, QWidget *parent) const
{
    const QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    const QString filename = QString("screenshot_%1.png").arg(datetime);
    const QString dst = Dialogs::getSaveImageFilename(filename, parent);
    if (!dst.isEmpty()) {
        auto screenshot = new Adb::Screenshot(dst, serial, parent);
        app->connect(screenshot, &Adb::Screenshot::finished, parent, [=](bool success) {
            if (success) {
                RememberDialog::say("ScreenshotSuccess", tr("Screenshot has been successfully created!"), parent);
            } else {
                QMessageBox::warning(parent, {}, tr("Could not take a screenshot."));
            }
            screenshot->deleteLater();
        });
        screenshot->run();
    }
}

QAction *ActionProvider::getOpenApk(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("document-open"), {}, parent);
    action->setShortcut(QKeySequence::Open);

    auto translate = [=]() { action->setText(tr("&Open APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getOptimizeApk(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("apk-optimize"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Optimize External APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getSignApk(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("apk-sign"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Sign External APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getInstallApk(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("apk-install"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+Shift+I"));

    auto translate = [=]() { action->setText(tr("&Install External APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getFind(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("edit-find"), {}, parent);
    action->setShortcut(QKeySequence::Find);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    auto translate = [=]() { action->setText(tr("&Find")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getFindNext(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("go-down-search"), {}, parent);
    action->setShortcut(QKeySequence::FindNext);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    auto translate = [=]() { action->setText(tr("Find &Next")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getFindPrevious(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("go-up-search"), {}, parent);
    action->setShortcut(QKeySequence::FindPrevious);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    auto translate = [=]() { action->setText(tr("Find Pre&vious")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getReplace(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("edit-find-replace"), {}, parent);
    action->setShortcut(QKeySequence::Replace);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    auto translate = [=]() { action->setText(tr("Find and &Replace")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getZoomIn(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("zoom-in"), {}, parent);
    action->setShortcut(QKeySequence::ZoomIn);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    auto translate = [=]() { action->setText(tr("Zoom In")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getZoomOut(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("zoom-out"), {}, parent);
    action->setShortcut(QKeySequence::ZoomOut);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    auto translate = [=]() { action->setText(tr("Zoom Out")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getZoomReset(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("zoom-normal"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+/"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    auto translate = [=]() { action->setText(tr("Reset Zoom")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

QAction *ActionProvider::getVisitWebPage(QObject *parent) const
{
    auto action = new QAction(QIcon::fromTheme("help-website"), {}, parent);

    auto translate = [=]() { action->setText(tr("Visit &Website")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitWebPage);
    return action;
}

QAction *ActionProvider::getVisitSourcePage(QObject *parent) const
{
    auto action = new QAction(QIcon::fromTheme("help-source"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Source Code")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitSourcePage);
    return action;
}

QAction *ActionProvider::getVisitDonatePage(QObject *parent) const
{
    auto action = new QAction(QIcon::fromTheme("help-donate"), {}, parent);

    auto translate = [=]() { action->setText(tr("Make a &Donation")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitDonatePage);
    return action;
}

QAction *ActionProvider::getExit(QWidget *widget) const
{
    auto action = new QAction(QIcon::fromTheme("application-exit"), {}, widget);
    action->setShortcut(QKeySequence::Quit);
    action->setMenuRole(QAction::QuitRole);

    auto translate = [=]() { action->setText(tr("E&xit")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, widget, &QWidget::close);
    return action;
}

QAction *ActionProvider::getCheckUpdates(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("help-update"), {}, parent);

    auto translate = [=]() { action->setText(tr("Check for &Updates")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, parent, [=]() {
        checkUpdates(parent);
    });

    return action;
}

QAction *ActionProvider::getResetSettings(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("edit-delete"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Reset Settings...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, parent, [=]() {
        resetSettings(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenOptions(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("configure"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+P"));
    action->setMenuRole(QAction::PreferencesRole);

    auto translate = [=]() { action->setText(tr("&Options...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, parent, [=]() {
        openOptions(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenDeviceManager(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("smartphone"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+D"));

    //: This string refers to multiple devices (as in "Manager of devices").
    auto translate = [=]() { action->setText(tr("&Device Manager...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, parent, [=]() {
        openDeviceManager(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenKeyManager(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("apk-sign"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+K"));

    //: This string refers to multiple keys (as in "Manager of keys").
    auto translate = [=]() { action->setText(tr("&Key Manager...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, parent, [=]() {
        openKeyManager(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenAndroidExplorer(QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("tool-androidexplorer"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+Shift+X"));

    auto translate = [=]() { action->setText(tr("&Android Explorer...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, parent, [=]() {
        openAndroidExplorer(parent);
    });

    return action;
}

QAction *ActionProvider::getTakeScreenshot(QWidget *parent) const
{
    return getTakeScreenshot({}, parent);
}

QAction *ActionProvider::getTakeScreenshot(const QString &serial, QWidget *parent) const
{
    auto action = new QAction(QIcon::fromTheme("camera-photo"), {}, parent);

    auto translate = [=]() { action->setText(tr("Take &Screenshot...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    if (serial.isEmpty()) {
        connect(action, &QAction::triggered, parent, [=]() {
            takeScreenshot(parent);
        });
    } else {
        connect(action, &QAction::triggered, parent, [=]() {
            takeScreenshot(serial, parent);
        });
    }

    return action;
}

QMenu *ActionProvider::getLanguages(QWidget *parent) const
{
    auto menu = new QMenu(parent);
    auto actionGroup = new QActionGroup(parent);
    actionGroup->setExclusive(true);

    auto translate = [=]() {
        // Set title
        menu->setTitle(tr("&Language"));
        // Set icon
        const QString currentLocale = app->settings->getLanguage();
        QIcon flag(Utils::getLocaleFlag(currentLocale));
        menu->setIcon(flag);
        // Set check mark
        const auto actions = actionGroup->actions();
        for (QAction *action : actions) {
            if (action->property("locale") == currentLocale) {
                action->setChecked(true);
                break;
            }
        }
    };
    connect(this, &ActionProvider::languageChanged, menu, translate);
    translate();

    const QList<Language> languages = app->getLanguages();
    for (const Language &language : languages) {
        const QString localeTitle = language.getTitle();
        const QString localeCode = language.getCode();
        const QIcon localeFlag = language.getFlag();
        QAction *action = actionGroup->addAction(localeFlag, localeTitle);
        action->setCheckable(true);
        action->setProperty("locale", localeCode);
        connect(action, &QAction::triggered, [=]() {
            app->setLanguage(localeCode);
        });
    }
    menu->addActions(actionGroup->actions());

    return menu;
}

bool ActionProvider::event(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        emit languageChanged();
        return true;
    }
    return QObject::event(event);
}
