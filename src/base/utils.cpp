#include "base/utils.h"
#include <QDir>
#include <QIcon>
#include <QProcess>
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

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

bool Utils::copyFile(const QString &src, const QString &dst)
{
    if (src.isEmpty() || dst.isEmpty() || !QFile::exists(src)) {
        qWarning() << "Could not copy file: invalid path to file(s).";
        return false;
    }
    QFile::remove(dst);
    return QFile::copy(src, dst);
}

bool Utils::copyImage(const QString &src, const QString &dst)
{
    if (src.isEmpty() || dst.isEmpty() || !QFile::exists(src)) {
        qWarning() << "Could not copy image: invalid path to file(s).";
        return false;
    }
    const QString srcSuffix = QFileInfo(src).suffix();
    const QString dstSuffix = QFileInfo(dst).suffix();
    const bool sameFormats = !QString::compare(srcSuffix, dstSuffix, Qt::CaseInsensitive);
    if (sameFormats) {
        return copyFile(src, dst);
    }
    const bool isSrcReadableImage = isImageReadable(src);
    const bool isDstWritableImage = isImageWritable(dst);
    if (isSrcReadableImage && isDstWritableImage) {
        return QImage(src).save(dst);
    }
    qWarning() << "Could not replace image: format not supported.";
    return false;
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

QString Utils::getAndroidCodename(int api)
{
    // Read more: https://source.android.com/setup/start/build-numbers
    switch (api) {
    case 3:
        return "1.5 - Cupcake";
    case 4:
        return "1.6 - Donut";
    case 5:
        return "2.0 - Eclair";
    case 6:
        return "2.0.1 - Eclair";
    case 7:
        return "2.1 - Eclair";
    case 8:
        return "2.2.x - Froyo";
    case 9:
        return "2.3 - 2.3.2 - Gingerbread";
    case 10:
        return "2.3.3 - 2.3.7 - Gingerbread";
    case 11:
        return "3.0 - Honeycomb";
    case 12:
        return "3.1 - Honeycomb";
    case 13:
        return "3.2.x - Honeycomb";
    case 14:
        return "4.0.1 - 4.0.2 - Ice Cream Sandwich";
    case 15:
        return "4.0.3 - 4.0.4 - Ice Cream Sandwich";
    case 16:
        return "4.1.x - Jelly Bean";
    case 17:
        return "4.2.x - Jelly Bean";
    case 18:
        return "4.3.x - Jelly Bean";
    case 19:
        return "4.4 - 4.4.4 - KitKat";
    case 20:
        return "4.4 - 4.4.4 - KitKat Wear";
    case 21:
        return "5.0 - Lollipop";
    case 22:
        return "5.1 - Lollipop";
    case 23:
        return "6.0 - Marshmallow";
    case 24:
        return "7.0 - Nougat";
    case 25:
        return "7.1 - Nougat";
    case 26:
        return "8.0 - Oreo";
    case 27:
        return "8.1 - Oreo";
    case 28:
        return "9.0 - Pie";
    }
    return QString();
}
