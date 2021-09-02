#include <QCommandLineParser>
#include "base/application.h"
#include "tools/apktool.h"
#include "tools/keystore.h"
#include "windows/dialogs.h"
#include <QPixmapCache>

Application::Application(int &argc, char **argv) : SingleApplication(argc, argv, true)
{
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
#elif defined(Q_OS_LINUX)
    qDebug() << "Platform: Linux";
#endif
    qDebug() << qPrintable(QString("Qt %1").arg(QT_VERSION_STR));
    qDebug();

#ifdef Q_OS_LINUX
    setWindowIcon(QIcon::fromTheme("apk-editor-studio"));
#endif

    QIcon::setThemeName(Utils::isLightTheme() ? "apk-editor-studio" : "apk-editor-studio-dark");
}

Application::~Application()
{
    delete settings;
}

int Application::exec()
{
    QPixmapCache::setCacheLimit(1024 * 100); // 100 MiB

    settings = new Settings();

    Apktool::reset();

    QDir().mkpath(Apktool::getOutputPath());
    QDir().mkpath(Apktool::getFrameworksPath());

    setLanguage(settings->getLanguage());

    auto firstInstance = createNewInstance();
    processArguments(arguments(), firstInstance);
    connect(this, &SingleApplication::receivedMessage, this, [this](quint32, QByteArray message) {
        MainWindow *instance = nullptr;
        if (settings->getSingleInstance()) {
            instance = instances.last();
            instance->setWindowState((instance->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        } else {
            instance = createNewInstance();
        }
        instance->activateWindow();
        instance->raise();
        if (!message.isEmpty()) {
            processArguments(QStringList() << app->applicationFilePath() << QString(message).split('\n'), instance);
        }
    });

    return QApplication::exec();
}

QList<Language> Application::getLanguages()
{
    QList<Language> languages;
    languages.append(QString("%1.en.qm").arg(Utils::getAppTitleSlug()));

    const QDir directory(Utils::getSharedPath("resources/translations/"));
    QStringList paths = directory.entryList({QString("%1.*.qm").arg(Utils::getAppTitleSlug())});
    for (const QString &path : paths) {
        languages.append(Language(path));
    }

    return languages;
}

MainWindow *Application::createNewInstance()
{
    auto instance = new MainWindow(projects);
    instance->show();
    instances.append(instance);
    connect(instance, &MainWindow::destroyed, this, [=]() {
        instances.removeOne(instance);
    });
    return instance;
}

void Application::setLanguage(const QString &locale)
{
    removeTranslator(&translator);
    removeTranslator(&translatorQt);

    const QString path = Utils::getSharedPath("resources/translations");
    if (translator.load(QString("%1.%2").arg(Utils::getAppTitleSlug(), locale), path)) {
        translatorQt.load(QString("qt.%1").arg(locale), path);
        installTranslator(&translator);
        installTranslator(&translatorQt);
        setLayoutDirection(QLocale(locale).textDirection());
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
    return SingleApplication::event(event);
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
