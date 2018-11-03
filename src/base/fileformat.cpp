#include "base/fileformat.h"
#include "base/utils.h"
#include <QFileInfo>
#include <QMimeDatabase>

FileFormat FileFormat::fromMime(const QMimeType &mime, const QString &title)
{
    if (!mime.isValid() || mime.isDefault()) {
        return FileFormat();
    }
    FileFormat format;
    format.title = title.isEmpty() ? Utils::capitalize(mime.comment()) : title;
    format.extensions = mime.suffixes();
    format.valid = true;
    return format;
}

FileFormat FileFormat::fromFilename(const QString &filename, const QString &title)
{
    QMimeDatabase database;
    const QMimeType mimeType = database.mimeTypeForFile(filename, QMimeDatabase::MatchExtension);
    if (mimeType.isValid() && !mimeType.isDefault()) {
        return fromMime(mimeType, title);
    }
    const QString extension = QFileInfo(filename).suffix();
    if (!extension.isEmpty()) {
        FileFormat format;
        format.title = title.isEmpty() ? extension.toUpper() : title;
        format.extensions << extension;
        format.valid = true;
        return format;
    }
    return FileFormat();
}

FileFormat FileFormat::fromExtension(const QString &extension, const QString &title)
{
    return fromFilename('.' + extension, title);
}

void FileFormat::addExtension(const QString &extension)
{
    extensions << extension;
    valid = true;
}

bool FileFormat::hasExtension(const QString &extension) const
{
    return extensions.contains(extension);
}

const QStringList &FileFormat::getExtensions() const
{
    return extensions;
}

QString FileFormat::getFilterString() const
{
    QStringList globs;
    for (const QString &extension : extensions) {
        globs.append(QString("*.%1").arg(extension));
    }
    return QString("%1 (%2)").arg(title, globs.join(' '));
}

bool FileFormat::isValid() const
{
    return valid;
}
