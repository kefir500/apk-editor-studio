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
