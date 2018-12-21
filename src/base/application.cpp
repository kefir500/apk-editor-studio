#include "base/application.h"
#include "tools/apktool.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include <QDesktopServices>
#include <QDateTime>
#include <QFileOpenEvent>
#include <QPixmapCache>
#include <QPainter>
#include <QScreen>
#include <QDebug>

Application::Application(int &argc, char **argv) : QtSingleApplication(argc, argv)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());

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
    setWindowIcon(loadIcon("application.png"));
#endif
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

    Apktool apktool(getSharedPath("tools/apktool.jar"));
    apktool.reset();

    QDir().mkpath(settings->getOutputDirectory());
    QDir().mkpath(settings->getFrameworksDirectory());

    setLanguage(settings->getLanguage());

    MainWindow mainwindow;
    mainwindow.show();
    window = &mainwindow;
    setActivationWindow(window);

    QStringList args = arguments();
    if (args.size() > 1) {
        args.removeFirst();
        for (const QString &arg : args) {
            openApk(arg);
        }
    }

    connect(this, &Application::messageReceived, [this](const QString &message) {
        if (!message.isEmpty()) {
            window->setWindowState((window->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            window->activateWindow();
            window->raise();
            const QStringList paths = message.split('\n');
            for (const QString &path : paths) {
                openApk(path);
            }
        }
    });

    return QApplication::exec();
}

QString Application::getTitle() const
{
    return APPLICATION;
}

QString Application::getVersion() const
{
    return VERSION;
}

QString Application::getTitleNoSpaces() const
{
    return getTitle().toLower().replace(' ', '-');
}

QString Application::getTitleAndVersion() const
{
    return QString("%1 v%2").arg(getTitle(), getVersion());
}

QString Application::getExecutableDirectory() const
{
    return QApplication::applicationDirPath() + '/';
}

QString Application::getTemporaryPath(const QString &subdirectory) const
{
#ifndef PORTABLE
    const QString path = QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation), getTitleNoSpaces(), subdirectory);
#else
    const QString path = QString("%1/data/temp/%2").arg(getExecutableDirectory(), subdirectory);
#endif
    return QDir::cleanPath(path);
}

QString Application::getLocalConfigPath(const QString &subdirectory) const
{
#ifndef PORTABLE
    const QString path = QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation), getTitleNoSpaces(), subdirectory);
#else
    const QString path = QString("%1/data/%2").arg(getExecutableDirectory(), subdirectory);
#endif
    return QDir::cleanPath(path);
}

QString Application::getSharedPath(const QString &resource) const
{
#ifndef Q_OS_LINUX
    const QString path = getExecutableDirectory() + resource;
#else
    const QString path = QString("%1/../share/%2/%3").arg(getExecutableDirectory(), getTitleNoSpaces(), resource);
#endif
    return QDir::cleanPath(path);
}

QString Application::getBinaryPath(const QString &executable) const
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

QPixmap Application::getLocaleFlag(const QLocale &locale) const
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

QList<Language> Application::getLanguages() const
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

QColor Application::getColor(Color color) const
{
    switch (color) {
        case ColorLogoPrimary:   return QColor(150, 200, 75);  // #96C84B
        case ColorLogoSecondary: return QColor(195, 218, 108); // #C3DA6C
        case ColorAndroid:       return QColor(164, 198, 57);  // #A4C639
        case ColorHighlight: {
            QColor color = QPalette().color(QPalette::Highlight);
            color.setAlpha(30);
            return color;
        }
        case ColorBackgroundStart: return QColor(250, 255, 230);
        case ColorBackgroundEnd:   return QColor(240, 245, 220);
        case ColorSuccess: return QColor(235, 250, 200);
        case ColorWarning: return QColor(255, 255, 200);
        case ColorError:   return QColor(255, 200, 200);
    }
    return QColor();
}

int Application::scale(int value) const
{
    return value * scaleFactor;
}

QSize Application::scale(int width, int height) const
{
    return QSize(width, height) * scaleFactor;
}

QIcon Application::loadIcon(const QString &filename) const
{
    QIcon icon;
    QDirIterator it(getSharedPath("resources/icons"), {filename}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        icon.addFile(it.next());
    }
    return icon;
}

QPixmap Application::loadPixmap(const QString &filename) const
{
    return QPixmap(getSharedPath(QString("resources/icons/static/%1").arg(filename)));
}

QString Application::getWebPage() const
{
    return QString("https://kefir500.github.io/%1/").arg(getTitleNoSpaces());
}

QString Application::getSourcePage() const
{
    return QString("https://github.com/kefir500/%1/").arg(getTitleNoSpaces());
}

QString Application::getIssuesPage() const
{
    return getSourcePage() + "issues";
}

QString Application::getContactPage() const
{
    return getWebPage();
}

QString Application::getTranslatePage() const
{
    return "https://www.transifex.com/qminds/" + getTitleNoSpaces();
}

QString Application::getDonatePage() const
{
    return "https://www.paypal.me/kefir500";
}

QString Application::getJrePage() const
{
    return "https://www.java.com/en/download/manual.jsp";
}

QString Application::getJdkPage() const
{
    return "http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html";
}

QString Application::getUpdateUrl() const
{
    return getWebPage() + "/VERSION";
}

Project *Application::openApk(const QString &filename, bool unpack)
{
    Project *existing = projects.existing(filename);
    if (existing) {
        //: "v%1" will be replaced with a path to an APK.
        const QString question = tr("This APK is already open:\n%1\nDo you want to reopen it and lose any unsaved changes?").arg(existing->getOriginalPath());
        const int answer = QMessageBox::question(window, QString(), question);
        if (answer != QMessageBox::Yes) {
            return existing;
        }
        projects.close(existing);
    }

    Project *project = projects.open(filename, unpack);
    connect(project, &Project::unpacked, [=]() {
        addToRecent(project);
    });
    connect(project, &Project::packed, [=]() {
        addToRecent(project);
    });
    connect(project, &Project::installed, [=]() {
        addToRecent(project);
    });

    return project;
}

bool Application::installExternalApk()
{
    const QStringList paths = Dialogs::getOpenApkFilenames(window);
    if (paths.isEmpty()) {
        return false;
    }
    DeviceManager devices(window);
    const Device *device = devices.getTargetDevice();
    if (!device) {
        return false;
    }
    for (const QString &path : paths) {
        Project *project = openApk(path, false);
        project->install(device->getSerial());
    }
    return true;
}

bool Application::closeApk(Project *project)
{
    return projects.close(project);
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

bool Application::addToRecent(const Project *project)
{
    return recent->add(project->getOriginalPath(), project->getThumbnail().pixmap(scale(32, 32)));
}

bool Application::associate() const
{
#ifdef Q_OS_WIN
    QSettings registry("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat);
    if (!registry.isWritable()) {
        return false;
    }
    const QString format = "apk";
    const QString description = "Android Application Package";
    const QString progid = QString("%1.%2").arg(getTitleNoSpaces(), format);
    const QString executable = QString("\"%1\"").arg(QDir::toNativeSeparators(applicationFilePath()));
    registry.setValue(QString(".%1/Default").arg(format), progid);
    registry.setValue(progid + "/Default", description);
    registry.setValue(progid + "/Shell/Open/Command/Default", executable + " \"%1\"");
    registry.setValue(progid + "/DefaultIcon/Default", executable + ",0");
    return true;
#else
    return false;
#endif
}

void Application::visitWebPage() const
{
    QDesktopServices::openUrl(getWebPage());
}

void Application::visitContactPage() const
{
    QDesktopServices::openUrl(getContactPage());
}

void Application::visitTranslatePage() const
{
    QDesktopServices::openUrl(getTranslatePage());
}

void Application::visitDonatePage() const
{
    QDesktopServices::openUrl(getDonatePage());
}

void Application::visitJrePage() const
{
    QDesktopServices::openUrl(getJrePage());
}

void Application::visitJdkPage() const
{
    QDesktopServices::openUrl(getJdkPage());
}

bool Application::event(QEvent *event)
{
    // File open request on macOS
    if (event->type() == QEvent::FileOpen) {
        const QString filePath = static_cast<QFileOpenEvent *>(event)->file();
        openApk(filePath);
        return true;
    }
    return QApplication::event(event);
}
