#include <QCommandLineParser>
#include "base/application.h"
#include "apk/project.h"
#include "tools/apktool.h"
#include "tools/keystore.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include <QDateTime>
#include <QDebug>
#include <QFileOpenEvent>
#include <QPixmapCache>
#include <QScreen>

Application::Application(int &argc, char **argv) : QtSingleApplication(argc, argv)
{
    qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));

    setApplicationName(APPLICATION);
    setApplicationVersion(VERSION);

#if defined(QT_DEBUG)
    setApplicationDisplayName(QString("%1 v%2 - DEBUG").arg(APPLICATION, VERSION));
#elif defined(PORTABLE)
    setApplicationDisplayName(QString("%1 v%2 Portable").arg(APPLICATION, VERSION));
#else
    setApplicationDisplayName(QString("%1 v%2").arg(APPLICATION, VERSION));
#endif

    qDebug() << qPrintable(applicationDisplayName());
#if defined(Q_OS_WIN)
    qDebug() << "Platform: Windows";
#elif defined(Q_OS_OSX)
    qDebug() << "Platform: macOS";
#else
    qDebug() << "Platform: Unix";
#endif
    qDebug() << qPrintable(QString("Qt %1").arg(QT_VERSION_STR));
    qDebug();

#ifdef Q_OS_LINUX
    setWindowIcon(QIcon::fromTheme("apk-editor-studio"));
#endif

    QIcon::setThemeSearchPaths({Utils::getSharedPath("/resources/icons")});
    if (!Utils::isDarkTheme()) {
        theme_ = new LightTheme;
        QIcon::setThemeName("default");
    } else {
        theme_ = new DarkTheme;
        QIcon::setThemeName("dark");
    }
}

Application::~Application()
{
    delete settings;
    delete recent;
}

int Application::exec()
{
#ifndef Q_OS_OSX
    const qreal dpi = this->primaryScreen()->logicalDotsPerInch();
    scaleFactor = dpi / 100.0;
#else
    scaleFactor = 1;
#endif

    QPixmapCache::setCacheLimit(1024 * 100); // 100 MiB

    settings = new Settings();
    recent = new Recent("apk");

    Apktool::reset();

    QDir().mkpath(Apktool::getOutputPath());
    QDir().mkpath(Apktool::getFrameworksPath());

    setLanguage(settings->getLanguage());

    auto firstInstance = new MainWindow;
    firstInstance->show();
    instances.append(firstInstance);

    processArguments(arguments(), firstInstance);
    connect(this, &QtSingleApplication::messageReceived, this, [this](const QString &message) {
        MainWindow *instance = nullptr;
        if (settings->getSingleInstance()) {
            instance = instances.last();
            instance->setWindowState((instance->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        } else {
            instance = new MainWindow;
            instance->show();
            instances.append(instance);
        }
        instance->activateWindow();
        instance->raise();
        if (!message.isEmpty()) {
            processArguments(QStringList() << app->applicationFilePath() << message.split('\n'), instance);
        }
    });

    return QApplication::exec();
}

QList<Language> Application::getLanguages()
{
    QList<Language> languages;
    languages.append(QString("%1.en.qm").arg(Utils::getTitleNoSpaces()));

    const QDir directory(Utils::getSharedPath("resources/translations/"));
    QStringList paths = directory.entryList({QString("%1.*.qm").arg(Utils::getTitleNoSpaces())});
    for (const QString &path : paths) {
        languages.append(Language(path));
    }

    return languages;
}

const Theme *Application::theme() const
{
    return theme_;
}

int Application::scale(int value) const
{
    return static_cast<int>(value * scaleFactor);
}

qreal Application::scale(qreal value) const
{
    return value * scaleFactor;
}

QSize Application::scale(int width, int height) const
{
    return QSize(width, height) * scaleFactor;
}

void Application::setLanguage(const QString &locale)
{
    removeTranslator(&translator);
    removeTranslator(&translatorQt);

    QLocale::setDefault(locale);
    const QString path = Utils::getSharedPath("resources/translations");
    if (translator.load(QString("%1.%2").arg(Utils::getTitleNoSpaces(), locale), path)) {
        translatorQt.load(QString("qt.%1").arg(locale), path);
        installTranslator(&translator);
        installTranslator(&translatorQt);
        settings->setLanguage(locale);
    } else {
        settings->setLanguage("en");
    }
}

bool Application::event(QEvent *event)
{
    // File open request on macOS
    switch (event->type()) {
    case QEvent::FileOpen: {
        const QString filePath = static_cast<QFileOpenEvent *>(event)->file();
        actions.openApk(filePath, instances.last());
        return true;
    }
    case QEvent::LanguageChange:
        postEvent(&actions, new QEvent(QEvent::LanguageChange));
        break;
    default:
        break;
    }
    return QtSingleApplication::event(event);
}

void Application::processArguments(const QStringList &arguments, MainWindow *window)
{
    QCommandLineParser cli;
    QCommandLineOption optimizeOption(QStringList{"o", "optimize", "z", "zipalign"});
    QCommandLineOption signOption(QStringList{"s", "sign"});
    QCommandLineOption installOption(QStringList{"i", "install"});
    cli.addOption(optimizeOption);
    cli.addOption(signOption);
    cli.addOption(installOption);
    cli.parse(arguments);

    for (const QString &path : cli.positionalArguments()) {
        auto project = projects.add(path, window);
        if (project) {
            auto command = new Project::ProjectCommand(project);
            if (!cli.isSet(optimizeOption) && !cli.isSet(signOption) && !cli.isSet(installOption)) {
                command->add(project->createUnpackCommand(), true);
            } else {
                if (cli.isSet(optimizeOption)) {
                    command->add(project->createZipalignCommand(), true);
                }
                if (cli.isSet(signOption)) {
                    auto keystore = Keystore::get(window);
                    if (keystore) {
                        command->add(project->createSignCommand(keystore.get()), true);
                    }
                }
                if (cli.isSet(installOption)) {
                    const auto device = Dialogs::getInstallDevice(window);
                    if (!device.isNull()) {
                        command->add(project->createInstallCommand(device.getSerial()), true);
                    }
                }
            }
            command->run();
        }
    }
}
