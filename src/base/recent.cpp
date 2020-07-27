#include "base/recent.h"
#include "base/application.h"
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

Recent::Recent(const QString &identifier, QObject *parent) : QObject(parent)
{
    this->identifier = identifier;
    this->thumbsPath = QString("%1/%2/").arg(Utils::getLocalConfigPath("recent/thumbnails"), identifier);
    this->limit = app->settings->getRecentLimit();

    QSettings ini(Utils::getLocalConfigPath("recent/recent.ini"), QSettings::IniFormat);
    ini.beginGroup("Recent");
    QStringList files = ini.value(identifier).toStringList();
    ini.endGroup();

    for (const QString &file : files) {
        recent.append(RecentFile(file, thumbnailPath(file)));
    }
    emit changed();
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

    saveToFile();
    emit changed();
    return true;
}

bool Recent::remove(int index)
{
    if (index >= recent.size()) {
        return false;
    }
    const QString filename = recent[index].filename();
    QFile::remove(thumbnailPath(filename));
    recent.removeAt(index);
    saveToFile();
    emit changed();
    return true;
}

void Recent::clear()
{
    while (!recent.isEmpty()) {
        remove(0);
    }
}

void Recent::setLimit(int limit)
{
    this->limit = limit;
    while (recent.size() > limit) {
        remove(recent.size() - 1);
    }
    app->settings->setRecentLimit(limit);
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
