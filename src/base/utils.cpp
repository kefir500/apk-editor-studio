#include "base/utils.h"
#include <QFileInfo>
#include <QImageWriter>
#include <QImageReader>
#include <QDebug>

QString Utils::capitalize(QString string)
{
    if (!string.isEmpty()) {
        string[0] = string[0].toUpper();
    }
    return string;
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
    const bool isSrcReadableImage = QImageReader::supportedImageFormats().contains(srcSuffix.toLocal8Bit());
    const bool isDstWritableImage = QImageWriter::supportedImageFormats().contains(dstSuffix.toLocal8Bit());
    if (isSrcReadableImage && isDstWritableImage) {
        return QImage(src).save(dst);
    }
    qWarning() << "Could not replace image: format not supported.";
    return false;
}
