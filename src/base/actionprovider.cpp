#include "base/actionprovider.h"
#include "base/application.h"
#include "windows/androidexplorer.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "windows/keymanager.h"
#include "windows/optionsdialog.h"
#include "tools/adb.h"
#include <QMenu>
#include <QDesktopServices>
#include <QDateTime>

void ActionProvider::openApk(QWidget *parent)
{
    const QStringList paths = Dialogs::getOpenApkFilenames(parent);
    openApk(paths, parent);
}

void ActionProvider::openApk(const QString &path, QWidget *parent)
{
    openApk(QStringList(path), parent);
}

void ActionProvider::openApk(const QStringList &paths, QWidget *parent)
{
    for (const QString &path : paths) {
        auto project = app->projects.add(path, parent);
        if (project) {
            auto command = new Project::ProjectCommand(project);
            command->add(project->createUnpackCommand(), true);
            command->run();
        }
    }
}

void ActionProvider::optimizeApk(QWidget *parent)
{
    const QStringList paths = Dialogs::getOpenApkFilenames(parent);
    optimizeApk(paths, parent);
}

void ActionProvider::optimizeApk(const QStringList &paths, QWidget *parent)
{
    for (const QString &path : paths) {
        auto project = app->projects.add(path, parent);
        if (project) {
            auto command = new Project::ProjectCommand(project);
            command->add(project->createZipalignCommand(), true);
            command->run();
        }
    }
}

void ActionProvider::signApk(QWidget *parent)
{
    const QStringList paths = Dialogs::getOpenApkFilenames(parent);
    signApk(paths, parent);
}

void ActionProvider::signApk(const QStringList &paths, QWidget *parent)
{
    const auto keystore = Keystore::get(parent);
    if (keystore) {
        signApk(paths, keystore.data(), parent);
    }
}

void ActionProvider::signApk(const QStringList &paths, const Keystore *keystore, QWidget *parent)
{
    Q_ASSERT(keystore);
    for (const QString &path : paths) {
        auto project = app->projects.add(path, parent);
        if (project) {
            auto command = new Project::ProjectCommand(project);
            command->add(project->createSignCommand(keystore), true);
            command->run();
        }
    }
}

void ActionProvider::installApk(QWidget *parent)
{
    const auto device = Dialogs::getInstallDevice(parent);
    if (device) {
        installApk(device->getSerial(), parent);
    }
}

void ActionProvider::installApk(const QString &serial, QWidget *parent)
{
    const QStringList paths = Dialogs::getOpenApkFilenames(parent);
    installApk(paths, serial, parent);
}

void ActionProvider::installApk(const QStringList &paths, const QString &serial, QWidget *parent)
{
    for (const QString &path : paths) {
        auto project = app->projects.add(path, parent);
        if (project) {
            auto command = new Project::ProjectCommand(project);
            command->add(project->createInstallCommand(serial), true);
            command->run();
        }
    }
}

bool ActionProvider::closeApk(Project *project)
{
    return app->projects.close(project);
}

void ActionProvider::visitWebPage()
{
    QDesktopServices::openUrl(app->getWebPage());
}

void ActionProvider::visitSourcePage()
{
    QDesktopServices::openUrl(app->getSourcePage());
}

void ActionProvider::visitDonatePage()
{
    QDesktopServices::openUrl(app->getDonatePage());
}

void ActionProvider::visitUpdatePage()
{
    QDesktopServices::openUrl(app->getUpdatePage());
}

void ActionProvider::visitBlogPage(const QString &post)
{
    const QString url = app->getWebPage() + "blog/" + post + "/";
    QDesktopServices::openUrl(url);
}

void ActionProvider::exit()
{
    app->window->close();
}

void ActionProvider::addToRecent(const Project *project) const
{
    const auto path = project->getOriginalPath();
    const auto icon = project->getThumbnail().pixmap(app->scale(32, 32));
    app->recent->add(path, icon);
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

void ActionProvider::openOptions(QWidget *parent)
{
    OptionsDialog settings(parent);
    settings.exec();
}

void ActionProvider::openDeviceManager(QWidget *parent)
{
    DeviceManager deviceManager(parent);
    deviceManager.exec();
}

void ActionProvider::openKeyManager(QWidget *parent)
{
    KeyManager keyManager(parent);
    keyManager.exec();
}

void ActionProvider::openAndroidExplorer(QWidget *parent)
{
    const auto device = Dialogs::getExplorerDevice(parent);
    if (device) {
        openAndroidExplorer(device->getSerial(), parent);
    }
}

void ActionProvider::openAndroidExplorer(const QString &serial, QWidget *parent)
{
    auto explorer = new AndroidExplorer(serial, parent);
    explorer->setAttribute(Qt::WA_DeleteOnClose);
    explorer->show();
}

void ActionProvider::takeScreenshot(QWidget *parent)
{
    const auto device = Dialogs::getScreenshotDevice(parent);
    if (device) {
        takeScreenshot(device->getSerial(), parent);
    }
}

void ActionProvider::takeScreenshot(const QString &serial, QWidget *parent)
{
    const QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    const QString filename = QString("screenshot_%1.png").arg(datetime);
    const QString dst = Dialogs::getSaveImageFilename(filename, parent);
    if (!dst.isEmpty()) {
        auto screenshot = new Adb::Screenshot(dst, serial, parent);
        app->connect(screenshot, &Adb::Screenshot::finished, [=](bool success) {
            if (!success) {
                QMessageBox::warning(parent, {}, tr("Could not take a screenshot."));
            }
            screenshot->deleteLater();
        });
        screenshot->run();
    }
}

QAction *ActionProvider::getOpenApk(QWidget *parent)
{
    auto action = new QAction(app->icons.get("open.png"), {}, parent);
    action->setShortcut(QKeySequence::Open);

    auto translate = [=]() { action->setText(tr("&Open APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        openApk(parent);
    });
    return action;
}

QAction *ActionProvider::getOptimizeApk(QWidget *parent)
{
    auto action = new QAction(app->icons.get("optimize.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Optimize External APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        optimizeApk(parent);
    });

    return action;
}

QAction *ActionProvider::getSignApk(QWidget *parent)
{
    auto action = new QAction(app->icons.get("key.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Sign External APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        signApk(parent);
    });

    return action;
}

QAction *ActionProvider::getInstallApk(QWidget *parent)
{
    return getInstallApk({}, parent);
}

QAction *ActionProvider::getInstallApk(const QString &serial, QWidget *parent)
{
    auto action = new QAction(app->icons.get("install.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Install External APK...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    action->setShortcut(QKeySequence("Ctrl+Shift+I"));
    if (serial.isEmpty()) {
        connect(action, &QAction::triggered, [=]() {
            installApk(parent);
        });
    } else {
        connect(action, &QAction::triggered, [=]() {
            installApk(serial, parent);
        });
    }

    return action;
}

QAction *ActionProvider::getVisitWebPage(QObject *parent)
{
    auto action = new QAction(app->icons.get("website.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Visit &Website")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitWebPage);
    return action;
}

QAction *ActionProvider::getVisitSourcePage(QObject *parent)
{
    auto action = new QAction(app->icons.get("github.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Source Code")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitSourcePage);
    return action;
}

QAction *ActionProvider::getVisitDonatePage(QObject *parent)
{
    auto action = new QAction(app->icons.get("donate.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Make a &Donation")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::visitDonatePage);
    return action;
}

QAction *ActionProvider::getExit(QObject *parent)
{
    auto action = new QAction(app->icons.get("x-red.png"), {}, parent);
    action->setShortcut(QKeySequence::Quit);
    action->setMenuRole(QAction::QuitRole);

    auto translate = [=]() { action->setText(tr("E&xit")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, this, &ActionProvider::exit);
    return action;
}

QAction *ActionProvider::getCheckUpdates(QWidget *parent)
{
    auto action = new QAction(app->icons.get("update.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Check for &Updates")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        checkUpdates(parent);
    });

    return action;
}

QAction *ActionProvider::getResetSettings(QWidget *parent)
{
    auto action = new QAction(app->icons.get("x-red.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Reset Settings...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        resetSettings(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenOptions(QWidget *parent)
{
    auto action = new QAction(app->icons.get("settings.png"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+P"));
    action->setMenuRole(QAction::PreferencesRole);

    auto translate = [=]() { action->setText(tr("&Options...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        openOptions(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenDeviceManager(QWidget *parent)
{
    auto action = new QAction(app->icons.get("devices.png"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+D"));

    //: This string refers to multiple devices (as in "Manager of devices").
    auto translate = [=]() { action->setText(tr("&Device Manager...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        openDeviceManager(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenKeyManager(QWidget *parent)
{
    auto action = new QAction(app->icons.get("key.png"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+K"));

    //: This string refers to multiple keys (as in "Manager of keys").
    auto translate = [=]() { action->setText(tr("&Key Manager...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, [=]() {
        openKeyManager(parent);
    });

    return action;
}

QAction *ActionProvider::getOpenAndroidExplorer(QWidget *parent)
{
    return getOpenAndroidExplorer({}, parent);
}

QAction *ActionProvider::getOpenAndroidExplorer(const QString &serial, QWidget *parent)
{
    auto action = new QAction(app->icons.get("explorer.png"), {}, parent);
    action->setShortcut(QKeySequence("Ctrl+Shift+X"));

    auto translate = [=]() { action->setText(tr("&Android Explorer...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    if (serial.isEmpty()) {
        connect(action, &QAction::triggered, [=]() {
            openAndroidExplorer(parent);
        });
    } else {
        connect(action, &QAction::triggered, [=]() {
            openAndroidExplorer(serial, parent);
        });
    }

    return action;
}

QAction *ActionProvider::getTakeScreenshot(QWidget *parent)
{
    return getTakeScreenshot({}, parent);
}

QAction *ActionProvider::getTakeScreenshot(const QString &serial, QWidget *parent)
{
    auto action = new QAction(app->icons.get("screenshot.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("Take &Screenshot...")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    if (serial.isEmpty()) {
        connect(action, &QAction::triggered, [=]() {
            takeScreenshot(parent);
        });
    } else {
        connect(action, &QAction::triggered, [=]() {
            takeScreenshot(serial, parent);
        });
    }

    return action;
}

QMenu *ActionProvider::getLanguages(QWidget *parent)
{
    auto menu = new QMenu(parent);
    auto actions = new QActionGroup(parent);
    actions->setExclusive(true);

    auto translate = [=]() {
        // Set title
        menu->setTitle(tr("&Language"));
        // Set icon
        const QString currentLocale = app->settings->getLanguage();
        QIcon flag(app->getLocaleFlag(currentLocale));
        menu->setIcon(flag);
        // Set check mark
        for (QAction *action : actions->actions()) {
            if (action->property("locale") == currentLocale) {
                action->setChecked(true);
                break;
            }
        }
    };
    connect(this, &ActionProvider::languageChanged, menu, translate);
    translate();

    QList<Language> languages = app->getLanguages();
    for (const Language &language : languages) {
        const QString localeTitle = language.getTitle();
        const QString localeCode = language.getCode();
        const QPixmap localeFlag = language.getFlag();
        QAction *action = actions->addAction(localeFlag, localeTitle);
        action->setCheckable(true);
        action->setProperty("locale", localeCode);
        connect(action, &QAction::triggered, [=]() {
            app->setLanguage(localeCode);
        });
    }
    menu->addActions(actions->actions());

    return menu;
}

QMenu *ActionProvider::getRecent(QWidget *parent)
{
    auto menuRecent = new QMenu(parent);
    menuRecent->setIcon(app->icons.get("recent.png"));

    auto translate = [=]() { menuRecent->setTitle(tr("Open &Recent")); };
    connect(this, &ActionProvider::languageChanged, menuRecent, translate);
    translate();

    auto initialize = [=]() {
        menuRecent->clear();
        auto recentList = app->recent->all();
        for (const RecentFile &recentEntry : recentList) {
            QAction *action = new QAction(recentEntry.thumbnail(), recentEntry.filename(), parent);
            menuRecent->addAction(action);
            connect(action, &QAction::triggered, [=]() {
                openApk(recentEntry.filename(), parent);
            });
        }
        menuRecent->addSeparator();
        menuRecent->addAction(recentList.isEmpty() ? getNoRecent(parent) : getClearRecent(parent));
    };
    connect(app->recent, &Recent::changed, initialize);
    initialize();

    return menuRecent;
}

QAction *ActionProvider::getClearRecent(QObject *parent)
{
    auto action = new QAction(app->icons.get("x-red.png"), {}, parent);

    auto translate = [=]() { action->setText(tr("&Clear List")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    connect(action, &QAction::triggered, app->recent, &Recent::clear);
    return action;
}

QAction *ActionProvider::getNoRecent(QObject *parent)
{
    auto action = new QAction(parent);
    action->setEnabled(false);

    auto translate = [=]() { action->setText(tr("No Recent Files")); };
    connect(this, &ActionProvider::languageChanged, action, translate);
    translate();

    return action;
}

bool ActionProvider::event(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        emit languageChanged();
        return true;
    }
    return QObject::event(event);
}
