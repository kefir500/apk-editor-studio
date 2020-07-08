#include <QCommandLineParser>
#include "base/application.h"
#include "apk/project.h"
#include "tools/apktool.h"
#include "tools/keystore.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include <QDateTime>
#include <QFileOpenEvent>
#include <QPixmapCache>
#include <QPainter>
#include <QScreen>
#include <QStandardPaths>
#include <QDebug>

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
    setWindowIcon(icons.get("application.png"));
#endif

    if (!Utils::isDarkTheme()) {
        theme_ = new LightTheme;
    } else {
        theme_ = new DarkTheme;
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

QString Application::getTitle()
{
    return APPLICATION;
}

QString Application::getVersion()
{
    return VERSION;
}

QString Application::getTitleNoSpaces()
{
    return getTitle().toLower().replace(' ', '-');
}

QString Application::getTitleAndVersion()
{
    return QString("%1 v%2").arg(getTitle(), getVersion());
}

QString Application::getExecutableDirectory()
{
    return applicationDirPath() + '/';
}

QString Application::getTemporaryPath(const QString &subdirectory)
{
#ifndef PORTABLE
    const QString path = QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation), getTitleNoSpaces(), subdirectory);
#else
    const QString path = QString("%1/data/temp/%2").arg(getExecutableDirectory(), subdirectory);
#endif
    return QDir::cleanPath(path);
}

QString Application::getLocalConfigPath(const QString &subdirectory)
{
#ifndef PORTABLE
    const QString path = QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation), getTitleNoSpaces(), subdirectory);
#else
    const QString path = QString("%1/data/%2").arg(getExecutableDirectory(), subdirectory);
#endif
    return QDir::cleanPath(path);
}

QString Application::getSharedPath(const QString &resource)
{
#ifndef Q_OS_LINUX
    const QString path = getExecutableDirectory() + resource;
#else
    const QString path = QString("%1/../share/%2/%3").arg(getExecutableDirectory(), getTitleNoSpaces(), resource);
#endif
    return QDir::cleanPath(path);
}

QString Application::getBinaryPath(const QString &executable)
{
#ifdef Q_OS_WIN
    QString path = getSharedPath("tools/" + executable);
#else
    const QString path = getExecutableDirectory() + executable;
#endif

    QFileInfo fileInfo(path);
#ifdef Q_OS_WIN
    if (fileInfo.suffix().isEmpty()) {
        path.append(".exe");
        fileInfo.setFile(path);
    }
#endif
    return fileInfo.exists() ? path : fileInfo.fileName();
}

QString Application::getJavaPath()
{
    const QString userPath = app->settings->getJavaPath();
    if (!userPath.isEmpty()) {
        return userPath;
    }
    const QString envPath = qgetenv("JAVA_HOME");
    if (!envPath.isEmpty()) {
        return envPath;
    }
    return QString();
}

QString Application::getJavaBinaryPath(const QString &executable)
{
    const QString javaPath = getJavaPath();
    if (!javaPath.isEmpty()) {
        return QDir(javaPath).filePath(QString("bin/%1").arg(executable));
    }
    return executable;
}

QPixmap Application::getLocaleFlag(const QLocale &locale)
{
    const QLocale::Language localeLanguage = locale.language();
    const QLocale::Country localeCountry = locale.country();
    const QStringList localeSegments = QLocale(localeLanguage, localeCountry).name().split('_');
    if (localeSegments.count() > 1) {
        QPixmap flag(QString(getSharedPath("resources/flags/%1.png")).arg(localeSegments.at(1).toLower()));
        const int flagWidth = flag.width();
        const int flagHeight = flag.height();
        const int longSide = qMax(flagWidth, flagHeight);
        QPixmap result(longSide, longSide);
        result.fill(Qt::transparent);
        QPainter painter(&result);
        painter.translate((longSide - flagWidth) / 2.0, (longSide - flagHeight) / 2.0);
        painter.drawPixmap(0, 0, flag);
        return result;
    } else {
        return QPixmap();
    }
}

QList<Language> Application::getLanguages()
{
    QList<Language> languages;
    languages.append(QString("%1.en.qm").arg(getTitleNoSpaces()));

    const QDir directory(getSharedPath("resources/translations/"));
    QStringList paths = directory.entryList({QString("%1.*.qm").arg(getTitleNoSpaces())});
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

QString Application::getWebPage()
{
    return QString("https://qwertycube.com/%1/").arg(getTitleNoSpaces());
}

QString Application::getUpdatePage()
{
    return QString("https://qwertycube.com/%1/#utm_campaign=update&utm_source=%1&utm_medium=application").arg(getTitleNoSpaces());
}

QString Application::getSourcePage()
{
    return QString("https://github.com/kefir500/%1/").arg(getTitleNoSpaces());
}

QString Application::getIssuesPage()
{
    return getSourcePage() + "issues";
}

QString Application::getContactPage()
{
    return getWebPage();
}

QString Application::getTranslatePage()
{
    return QString("https://www.transifex.com/qwertycube/%1/").arg(getTitleNoSpaces());
}

QString Application::getDonatePage()
{
    return QString("https://qwertycube.com/donate/#utm_campaign=donate&utm_source=%1&utm_medium=application").arg(getTitleNoSpaces());
}

QString Application::getJrePage()
{
    return "https://www.java.com/en/download/manual.jsp";
}

QString Application::getJdkPage()
{
    return "http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html";
}

QString Application::getUpdateUrl()
{
    return getWebPage() + "/versions.json";
}

void Application::setLanguage(const QString &locale)
{
    removeTranslator(&translator);
    removeTranslator(&translatorQt);

    QLocale::setDefault(locale);
    const QString path = getSharedPath("resources/translations");
    if (translator.load(QString("%1.%2").arg(getTitleNoSpaces(), locale), path)) {
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
