#include "base/utils.h"
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

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
