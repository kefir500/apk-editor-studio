#include "base/fileformatlist.h"
#include <QMimeDatabase>
#include <QImageReader>
#include <QImageWriter>

FileFormatList FileFormatList::forApk()
{
    FileFormatList filter;
    filter.add(FileFormat::fromExtension("apk"));
    return filter;
}

FileFormatList FileFormatList::forKeystore()
{
    FileFormatList filter;
    FileFormat format = FileFormat::fromExtension("jks");
    format.addExtension("keystore");
    filter.add(format);
    return filter;
}

FileFormatList FileFormatList::forReadableImages()
{
    FileFormatList filter;
    QMimeDatabase database;
    const QList<QByteArray> mimeList = QImageReader::supportedMimeTypes();
    for (const QByteArray &entry : mimeList) {
        const QMimeType mimeType = database.mimeTypeForName(entry);
        filter.add(FileFormat::fromMime(mimeType));
    }
    return filter;
}

FileFormatList FileFormatList::forWritableImages()
{
    FileFormatList filter;
    const QList<QByteArray> mimeList = QImageWriter::supportedMimeTypes();
    QMimeDatabase database;
    for (const QByteArray &entry : mimeList) {
        const QMimeType mimeType = database.mimeTypeForName(entry);
        filter.add(FileFormat::fromMime(mimeType));
    }
    return filter;
}

FileFormatList::FileFormatList(const FileFormat &format) : FileFormatList()
{
    add(format);
}

void FileFormatList::add(const FileFormat &format)
{
    if (format.isValid()) {
        formats.append(format);
    }
}

bool FileFormatList::isEmpty() const
{
    return formats.isEmpty();
}

QStringList FileFormatList::getExtensions() const
{
    QStringList extensions;
    for (const FileFormat &format : formats) {
        extensions.append(format.getExtensions());
    }
    return extensions;
}

QString FileFormatList::getFilterString() const
{
    QString filter;
    QStringList globs;
    for (const FileFormat &format : formats) {
        filter.append(format.getFilterString() + ";;");
        const QStringList extensions = format.getExtensions();
        for (const QString &extension : extensions) {
            globs.append(QString("*.%1").arg(extension));
        }
    }
    if (formats.size() > 1) {
        filter.prepend(QString("%1 (%2);;").arg(tr("All supported files"), globs.join(' ')));
    }
    filter.append(QString("%1 (*)").arg(tr("All files")));
    return filter;
}
