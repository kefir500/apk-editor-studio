#include <QCommandLineParser>
#include "base/application.h"
#include "base/settings.h"
#include "base/themes.h"
#include "base/utils.h"
#include "tools/apktool.h"
#include "tools/keystore.h"
#include "windows/androidexplorer.h"
#include "windows/dialogs.h"
#include "windows/mainwindow.h"
#include <QDir>
#include <QFileOpenEvent>
#include <QPixmapCache>
#include <QTimer>

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
#elif defined(Q_OS_MACOS)
    qDebug() << "Platform: macOS";
#elif defined(Q_OS_LINUX)
    qDebug() << "Platform: Linux";
#endif
    qDebug() << qPrintable(QString("Qt %1").arg(QT_VERSION_STR));
    qDebug();

#ifdef Q_OS_LINUX
    setWindowIcon(QIcon::fromTheme("apk-editor-studio"));
#endif
}

Application::~Application()
{
    delete settings;
}

int Application::exec()
{
    Q_ASSERT(instances.isEmpty());
    settings = new Settings();
    setLanguage(settings->getLanguage());
    setTheme(settings->getTheme());
    connect(this, &SingleApplication::receivedMessage, this, &Application::start);
    start();
    return QApplication::exec();
}

QList<Language> Application::getLanguages()
{
    QList<Language> languages;
    languages.append(QString("%1.en.qm").arg(Utils::getAppTitleSlug()));

    const QDir directory(Utils::getSharedPath("resources/translations/"));
    const QStringList paths = directory.entryList({QString("%1.*.qm").arg(Utils::getAppTitleSlug())});
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

void Application::setTheme(const QString &theme)
{
    ThemeRepository().getTheme(theme)->initialize();
}

bool Application::event(QEvent *event)
{
    // File open request on macOS
    switch (event->type()) {
    case QEvent::FileOpen: {
        const QString filePath = static_cast<QFileOpenEvent *>(event)->file();
        instances.last()->openApk(filePath);
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

void Application::start(quint32 instanceId, QByteArray message)
{
    const QStringList args = message.isEmpty()
        ? arguments()
        : QStringList(QString(message).split('\n'));
    QCommandLineParser cli;
    QCommandLineOption explorerOption(QStringList{"e", "explorer"});
    cli.addOption(explorerOption);
    cli.parse(args);
    if (cli.isSet(explorerOption)) {
        startExplorer();
    } else if (instances.isEmpty()) {
        startStudio(args);
    } else {
        startStudioInstance(args);
    }
}

void Application::startStudio(const QStringList &args)
{
    qDebug() << "Starting APK Editor Studio Studio...";
    Apktool::reset();
    QDir().mkpath(Apktool::getOutputPath());
    QDir().mkpath(Apktool::getFrameworksPath());
    QPixmapCache::setCacheLimit(1024 * 100); // 100 MiB

    auto firstInstance = createNewInstance();
    firstInstance->processArguments(args);
}

void Application::startStudioInstance(const QStringList &args)
{
    qDebug() << "Starting APK Editor Studio instance...";
    MainWindow *instance = nullptr;
    if (settings->getSingleInstance()) {
        instance = instances.last();
        instance->setWindowState((instance->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    } else {
        instance = createNewInstance();
    }
    instance->activateWindow();
    instance->raise();
    instance->processArguments(args);
}

void Application::startExplorer()
{
    qDebug() << "Starting Android Explorer...";
    const auto device = Dialogs::getExplorerDevice();
    if (!device.isNull()) {
        auto explorer = new AndroidExplorer(device.getSerial());
        explorer->setAttribute(Qt::WA_DeleteOnClose);
        explorer->show();
    } else if (allWindows().isEmpty()) {
        QTimer::singleShot(0, this, &QApplication::quit);
    }
}
