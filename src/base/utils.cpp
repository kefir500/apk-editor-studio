#include "base/utils.h"
#include "windows/dialogs.h"
#include <QDir>
#include <QIcon>
#include <QPainter>
#include <QProcess>
#include <QImageReader>
#include <QImageWriter>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrent>
#include "base/application.h"

QString Utils::capitalize(QString string)
{
    if (!string.isEmpty()) {
        string[0] = string[0].toUpper();
    }
    return string;
}

int Utils::roundToNearest(int number, QList<int> numbers)
{
    if (numbers.isEmpty()) {
        return number;
    }
    std::sort(numbers.begin(), numbers.end());
    const int first = numbers.at(0);
    const int last = numbers.at(numbers.size() - 1);
    if (number <= first) {
        return first;
    }
    if (number >= last) {
        return last;
    }
    for (int i = 0; i < numbers.count() - 1; ++i) {
        const int prev = numbers.at(i);
        const int next = numbers.at(i + 1);
        if (number < prev || number > next) {
            // Number is not in the range; continue loop
            continue;
        }
        const qreal middle = (prev + next) / 2.0;
        if (number <= middle) {
            return prev;
        } else {
            return next;
        }
    }
    return number;
}

bool Utils::isDark(const QColor &color)
{
    return color.lightness() < 127;
}

bool Utils::isDarkTheme()
{
    return isDark(QPalette().color(QPalette::Base));
}

bool Utils::explore(const QString &path)
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
    const QString action = fileInfo.isDir() ? "open" : "reveal";
    arguments << "-e"
              << QString("tell application \"Finder\" to %1 POSIX file \"%2\"").arg(action, fileInfo.canonicalFilePath());
    QProcess::execute(QLatin1String("/usr/bin/osascript"), arguments);
    arguments.clear();
    arguments << "-e"
              << QString("tell application \"Finder\" to activate");
    QProcess::execute(QLatin1String("/usr/bin/osascript"), arguments);
    return true;
#else
    const QString directory = fileInfo.isDir() ? fileInfo.canonicalFilePath() : fileInfo.canonicalPath();
    return QDesktopServices::openUrl(QUrl::fromLocalFile(directory));
#endif
}

void Utils::rmdir(const QString &path, bool recursive)
{
    if (!recursive) {
        QDir().rmdir(path);
    } else if (!path.isEmpty()) {
        QtConcurrent::run([=]() {
            QDir(path).removeRecursively();
        });
    }
}

namespace
{
    bool copy(const QString &src, const QString &dst)
    {
        if (src.isEmpty() || dst.isEmpty() || !QFile::exists(src)) {
            qWarning() << "Could not copy file: invalid path to file(s).";
            return false;
        }
        if (QFileInfo(src) == QFileInfo(dst)) {
            qWarning() << "Could not copy file: source and destination paths are identical.";
            return false;
        }
        const QString srcSuffix = QFileInfo(src).suffix();
        const QString dstSuffix = QFileInfo(dst).suffix();
        const bool sameFormats = !QString::compare(srcSuffix, dstSuffix, Qt::CaseInsensitive);
        const bool isSrcReadableImage = Utils::isImageReadable(src);
        const bool isDstWritableImage = Utils::isImageWritable(dst);
        if (!(isSrcReadableImage && isDstWritableImage && !sameFormats)) {
            QFile::remove(dst);
            return QFile::copy(src, dst);
        } else {
            return QImage(src).save(dst);
        }
    }
}

bool Utils::copyFile(const QString &src, QWidget *parent)
{
    return copyFile(src, QString(), parent);
}

bool Utils::copyFile(const QString &src, QString dst, QWidget *parent)
{
    if (dst.isNull()) {
        dst = isImageReadable(src)
            ? Dialogs::getSaveImageFilename(src, parent)
            : Dialogs::getSaveFilename(src, parent);
    }
    if (dst.isEmpty()) {
        return false;
    }
    if (!copy(src, dst)) {
        QMessageBox::warning(parent, QString(), app->translate("Utils", "Could not save the file."));
        return false;
    }
    return true;
}

bool Utils::replaceFile(const QString &what, QWidget *parent)
{
    return replaceFile(what, QString(), parent);
}

bool Utils::replaceFile(const QString &what, QString with, QWidget *parent)
{
    if (with.isNull()) {
        with = isImageWritable(what)
            ? Dialogs::getOpenImageFilename(what, parent)
            : Dialogs::getOpenFilename(what, parent);
    }
    if (with.isEmpty() || QFileInfo(with) == QFileInfo(what)) {
        return false;
    }
    if (!copy(with, what)) {
        QMessageBox::warning(parent, QString(), app->translate("Utils", "Could not replace the file."));
        return false;
    }
    return true;
}

QString Utils::normalizePath(QString path)
{
    return path.replace(QRegularExpression("^\\/+[\\.\\./*]*\\/*|$"), "/");
}

bool Utils::isImageReadable(const QString &path)
{
    const QString extension = QFileInfo(path).suffix();
    return QImageReader::supportedImageFormats().contains(extension.toLocal8Bit());
}

bool Utils::isImageWritable(const QString &path)
{
    const QString extension = QFileInfo(path).suffix();
    return QImageWriter::supportedImageFormats().contains(extension.toLocal8Bit());
}

QPixmap Utils::iconToPixmap(const QIcon &icon)
{
    const auto sizes = icon.availableSizes();
    const QSize size = !sizes.isEmpty() ? sizes.first() : QSize();
    return icon.pixmap(size);
}

QString Utils::getTitle()
{
    return APPLICATION;
}

QString Utils::getVersion()
{
    return VERSION;
}

QString Utils::getTitleNoSpaces()
{
    return getTitle().toLower().replace(' ', '-');
}

QString Utils::getTitleAndVersion()
{
    return QString("%1 v%2").arg(getTitle(), getVersion());
}

QString Utils::getExecutableDirectory()
{
    return qApp->applicationDirPath() + '/';
}

QString Utils::getTemporaryPath(const QString &subdirectory)
{
#ifndef PORTABLE
    const QString path = QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation), getTitleNoSpaces(), subdirectory);
#else
    const QString path = QString("%1/data/temp/%2").arg(getExecutableDirectory(), subdirectory);
#endif
    return QDir::cleanPath(path);
}

QString Utils::getLocalConfigPath(const QString &subdirectory)
{
#ifndef PORTABLE
    const QString path = QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation), getTitleNoSpaces(), subdirectory);
#else
    const QString path = QString("%1/data/%2").arg(getExecutableDirectory(), subdirectory);
#endif
    return QDir::cleanPath(path);
}

QString Utils::getSharedPath(const QString &resource)
{
#ifndef Q_OS_LINUX
    const QString path = getExecutableDirectory() + resource;
#else
    const QString path = QString("%1/../share/%2/%3").arg(getExecutableDirectory(), getTitleNoSpaces(), resource);
#endif
    return QDir::cleanPath(path);
}

QString Utils::getBinaryPath(const QString &executable)
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

QString Utils::getJavaPath()
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

QString Utils::getJavaBinaryPath(const QString &executable)
{
    const QString javaPath = getJavaPath();
    if (!javaPath.isEmpty()) {
        return QDir(javaPath).filePath(QString("bin/%1").arg(executable));
    }
    return executable;
}

QIcon Utils::getLocaleFlag(const QLocale &locale)
{
    const QStringList localeSegments = locale.name().split('_');
    if (localeSegments.count() <= 1) {
        return QPixmap();
    }
    const QString countryCode = localeSegments.at(1).toLower();
    return QIcon::fromTheme(QString("intl/flag-%1").arg(countryCode));
}

QString Utils::getWebPage()
{
    return QString("https://qwertycube.com/%1/").arg(getTitleNoSpaces());
}

QString Utils::getUpdatePage()
{
    return QString("https://qwertycube.com/%1/#utm_campaign=update&utm_source=%1&utm_medium=application").arg(getTitleNoSpaces());
}

QString Utils::getSourcePage()
{
    return QString("https://github.com/kefir500/%1/").arg(getTitleNoSpaces());
}

QString Utils::getIssuesPage()
{
    return getSourcePage() + "issues";
}

QString Utils::getContactPage()
{
    return getWebPage();
}

QString Utils::getTranslatePage()
{
    return QString("https://www.transifex.com/qwertycube/%1/").arg(getTitleNoSpaces());
}

QString Utils::getDonatePage()
{
    return QString("https://qwertycube.com/donate/#utm_campaign=donate&utm_source=%1&utm_medium=application").arg(getTitleNoSpaces());
    // See utm_content if you'd like to differ between donate buttons
}

QString Utils::getJrePage()
{
    return "https://www.java.com/en/download/manual.jsp";
}

QString Utils::getJdkPage()
{
    return "http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html";
}

QString Utils::getUpdateUrl()
{
    return getWebPage() + "/versions.json";
}

QString Utils::getAndroidCodename(int api)
{
    // Read more: https://source.android.com/setup/start/build-numbers
    switch (api) {
    case ANDROID_3:
        return "1.5 - Cupcake";
    case ANDROID_4:
        return "1.6 - Donut";
    case ANDROID_5:
        return "2.0 - Eclair";
    case ANDROID_6:
        return "2.0.1 - Eclair";
    case ANDROID_7:
        return "2.1 - Eclair";
    case ANDROID_8:
        return "2.2.x - Froyo";
    case ANDROID_9:
        return "2.3 - 2.3.2 - Gingerbread";
    case ANDROID_10:
        return "2.3.3 - 2.3.7 - Gingerbread";
    case ANDROID_11:
        return "3.0 - Honeycomb";
    case ANDROID_12:
        return "3.1 - Honeycomb";
    case ANDROID_13:
        return "3.2.x - Honeycomb";
    case ANDROID_14:
        return "4.0.1 - 4.0.2 - Ice Cream Sandwich";
    case ANDROID_15:
        return "4.0.3 - 4.0.4 - Ice Cream Sandwich";
    case ANDROID_16:
        return "4.1.x - Jelly Bean";
    case ANDROID_17:
        return "4.2.x - Jelly Bean";
    case ANDROID_18:
        return "4.3.x - Jelly Bean";
    case ANDROID_19:
        return "4.4 - 4.4.4 - KitKat";
    case ANDROID_20:
        return "4.4 - 4.4.4 - KitKat Wear";
    case ANDROID_21:
        return "5.0 - Lollipop";
    case ANDROID_22:
        return "5.1 - Lollipop";
    case ANDROID_23:
        return "6.0 - Marshmallow";
    case ANDROID_24:
        return "7.0 - Nougat";
    case ANDROID_25:
        return "7.1 - Nougat";
    case ANDROID_26:
        return "8.0 - Oreo";
    case ANDROID_27:
        return "8.1 - Oreo";
    case ANDROID_28:
        return "9.0 - Pie";
    case ANDROID_29:
        return "Android 10";
    }
    return QString();
}

bool Utils::isDrawableResource(const QFileInfo &file)
{
    // Read more: https://developer.android.com/guide/topics/resources/drawable-resource.html
    const QStringList drawableFormats = {"png", "jpg", "jpeg", "gif", "xml", "webp"};
    return drawableFormats.contains(file.suffix());
}
