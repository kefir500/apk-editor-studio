#include "base/application.h"
#include "base/debug.h"
#include "tools/adb.h"
#include "tools/apktool.h"
#include "tools/java.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include <QDesktopServices>
#include <QFileOpenEvent>
#include <QPixmapCache>
#include <QMovie>
#include <QScreen>
#undef app
#include <QtConcurrent/QtConcurrent>

Application::Application(int &argc, char **argv) : QApplication(argc, argv)
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
    qDebug() << qPrintable(QString("Qt %1").arg(QT_VERSION_STR));
    qDebug();

#ifdef Q_OS_LINUX
    setWindowIcon(loadIcon("application.png"));
#endif

#ifndef Q_OS_OSX
    const qreal dpi = this->primaryScreen()->logicalDotsPerInch();
    scaleFactor = dpi / 100.0;
#else
    scaleFactor = 1;
#endif

    QPixmapCache::setCacheLimit(1024 * 100); // 100 MiB
}

Application::~Application()
{
    delete settings;
    delete recent;
}

int Application::exec()
{
    settings = new Settings();
    recent = new Recent("apk");

    setLanguage(settings->getLanguage());

    MainWindow mainwindow;
    mainwindow.show();
    window = &mainwindow;

    QStringList args = arguments();
    if (args.size() > 1) {
        args.removeFirst();
        foreach (const QString &arg, args) {
            openApk(arg);
        }
    }

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

QString Application::getDirectory() const
{
    return QApplication::applicationDirPath() + '/';
}

QString Application::getTemporaryPath() const
{
#ifndef PORTABLE
    return QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation), getTitle());
#else
    return QString("%1/data/temp").arg(app->getDirectory(), subdirectory);
#endif
}

QString Application::getOutputPath() const
{
    return QString("%1/apk").arg(getTemporaryPath());
}

QString Application::getFrameworksPath() const
{
    return QString("%1/frameworks").arg(getTemporaryPath());
}

QString Application::getLocalConfigPath(QString subdirectory) const
{
#ifndef PORTABLE
    return QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation), getTitle(), subdirectory);
#else
    return QString("%1/data/%2").arg(app->getDirectory(), subdirectory);
#endif
}

QString Application::getSharedPath(const QString &resource) const
{
#ifndef Q_OS_LINUX
    return getDirectory() + resource;
#else
    return QString("%1../share/%2/%3/").arg(getDirectory(), getTitleNoSpaces(), resource);
#endif
}

QString Application::getToolPath(const QString &tool) const
{
#ifdef Q_OS_WIN
    QFileInfo fileInfo(tool);
    const QString suffix = fileInfo.suffix();
    if (suffix.isEmpty()) {
        return getSharedPath() + "tools/" + tool + ".exe";
    }
#endif
    return getSharedPath() + "tools/" + tool;
}

QPixmap Application::getLocaleFlag(const QLocale &locale) const
{
    const QLocale::Language localeLanguage = locale.language();
    const QLocale::Country localeCountry = locale.country();
    const QStringList localeSegments = QLocale(localeLanguage, localeCountry).name().split('_');
    if (localeSegments.count() > 1) {
        return QPixmap(QString(getSharedPath("/resources/flags/%1.png")).arg(localeSegments.at(1).toLower()));
    } else {
        return QPixmap();
    }
}

QList<Language> Application::getLanguages() const
{
    QList<Language> languages;
    languages.append(QString("%1.en.qm").arg(getTitleNoSpaces()));

    const QDir directory(getSharedPath("resources/translations/"));
    QStringList paths = directory.entryList(QStringList() << QString("%1.*.qm").arg(getTitleNoSpaces()));
    foreach (const QString &path, paths) {
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
        case ColorBackgroundEnd:   return QColor(245, 250, 225);
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
    QDirIterator it(getSharedPath() + "/resources/icons", QStringList() << filename, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        icon.addFile(it.next());
    }
    return icon;
}

QPixmap Application::loadPixmap(const QString &filename) const
{
    return QPixmap(getSharedPath() + "/resources/icons/static/" + filename);
}

bool Application::replaceImage(const QString &targetPath, const QString &sourcePath) const
{
    if (targetPath.isEmpty() || sourcePath.isEmpty() || !QFile::exists(targetPath)) {
        qWarning() << "Could not replace image: invalid path to file(s).";
        return false;
    }
    if (!formats.extensionsImages().contains(QFileInfo(targetPath).suffix())) {
        qWarning() << "Could not replace image: target format is not supported.";
        return false;
    }
    return QImage(sourcePath).save(targetPath);
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
    return getWebPage() + "VERSION";
}

Project *Application::openApk(const QString &filename, bool unpack)
{
    Project *existing = projects.get(filename);
    if (existing) {
        const int answer = QMessageBox::question(window, QString(), tr("This APK is already open.\nDo you want to reopen it and lose any unsaved changes?"));
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
    const Device *device = devices.getDevice();
    if (!device) {
        return false;
    }
    foreach (const QString &path, paths) {
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
    const QString executable = QString("\"%1\"").arg(QDir::toNativeSeparators(QApplication::applicationFilePath()));
    registry.setValue(QString(".%1/Default").arg(format), progid);
    registry.setValue(progid + "/Default", description);
    registry.setValue(progid + "/Shell/Open/Command/Default", executable + " \"%1\"");
    registry.setValue(progid + "/DefaultIcon/Default", executable + ",0");
    return true;
#else
    return false;
#endif
}

bool Application::explore(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }
    const QFileInfo fileInfo(path);
#if defined(Q_OS_WIN)
    const QString nativePath = QDir::toNativeSeparators(fileInfo.canonicalFilePath());
    const QString argument = fileInfo.isDir() ? nativePath : QString("/select,%1").arg(nativePath);
    return QProcess::startDetached(QString("explorer.exe %1").arg(argument));
#elif defined(Q_OS_OSX)
    QStringList arguments;
    arguments << "-e"
              << QString("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(fileInfo.canonicalFilePath());
    QProcess::execute(QLatin1String("/usr/bin/osascript"), arguments);
    arguments.clear();
    arguments << "-e"
              << QString("tell application \"Finder\" to activate");
    QProcess::execute(QLatin1String("/usr/bin/osascript"), arguments);
    return true;
#else
    // TODO Check on Linux
    return QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
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
