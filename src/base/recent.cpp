#include "base/recent.h"
#include "base/utils.h"
#include <QCryptographicHash>
#include <QSettings>
#include <QDir>

RecentFile::RecentFile(const QString &filename, const QPixmap &thumbnail)
{
    d = new RecentFilePrivate(filename, thumbnail);
}

const QString &RecentFile::filename() const
{
    return d->filename;
}

const QPixmap &RecentFile::thumbnail() const
{
    return d->thumbnail;
}

bool RecentFile::operator==(const RecentFile &file) const
{
    return filename() == file.filename();
}

Recent::Recent(const QString &identifier, int limit, QObject *parent)
    : QObject(parent)
    , identifier(identifier)
    , thumbsPath(QString("%1/%2/").arg(Utils::getLocalConfigPath("recent/thumbnails"), identifier))
    , limit(limit)
{
    QSettings ini(Utils::getLocalConfigPath("recent/recent.ini"), QSettings::IniFormat);
    ini.beginGroup("Recent");
    QStringList files = ini.value(identifier).toStringList();
    ini.endGroup();

    for (const QString &file : files) {
        recent.append(RecentFile(file, thumbnailPath(file)));
    }
}

bool Recent::add(const QString &filename, const QPixmap &thumbnail)
{
    if (filename.isEmpty()) {
        return false;
    }

    // Save cached thumbnail:
    QDir().mkpath(thumbsPath);
    thumbnail.save(thumbnailPath(filename));

    // Remove duplicates:
    RecentFile recentFile(filename, thumbnail);
    recent.removeAll(recentFile);

    // Add recent entry:
    recent.prepend(recentFile);

    // Limit to maximum:
    while (recent.size() > limit) {
        remove(recent.size() - 1);
    }

    // Save recent list:
    saveToFile();

    return true;
}

bool Recent::remove(int index)
{
    if (index >= recent.size()) {
        return false;
    }

    // Remove thumbnail:
    const QString filename = recent[index].filename();
    QFile::remove(thumbnailPath(filename));

    // Remove recent entry:
    recent.removeAt(index);

    // Save recent list:
    saveToFile();

    return true;
}

void Recent::clear()
{
    while (!recent.isEmpty()) {
        remove(0);
    }
}

void Recent::setLimit(int value)
{
    limit = value;
    while (recent.size() > limit) {
        remove(recent.size() - 1);
    }
}

const QList<RecentFile> &Recent::all() const
{
    return recent;
}

QStringList Recent::filenames() const
{
    QStringList result;
    for (const RecentFile &entry : recent) {
        result << entry.filename();
    }
    return result;
}

QList<QPixmap> Recent::thumbnails() const
{
    QList<QPixmap> result;
    for (const RecentFile &entry : recent) {
        result << entry.thumbnail();
    }
    return result;
}

void Recent::saveToFile() const
{
    QSettings ini(Utils::getLocalConfigPath("recent/recent.ini"), QSettings::IniFormat);
    ini.beginGroup("Recent");
    ini.setValue(identifier, filenames());
    ini.endGroup();
}

QString Recent::thumbnailPath(const QString &filename) const
{
    return thumbsPath + hash(filename) + ".png";
}

QString Recent::hash(const QString &string) const
{
    return QCryptographicHash::hash(string.toUtf8(), QCryptographicHash::Md5).toHex();
}
