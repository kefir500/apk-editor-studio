#include "base/recentlist.h"
#include "base/utils.h"
#include <QCryptographicHash>
#include <QDir>
#include <QSettings>

RecentList::RecentList(const QString &identifier, int limit, QObject *parent)
    : QObject(parent)
    , identifier(identifier)
    , thumbsPath(QString("%1/%2/").arg(Utils::getLocalConfigPath("recent/thumbnails"), identifier))
    , limit(limit)
{
    QSettings ini(Utils::getLocalConfigPath("recent/recent.ini"), QSettings::IniFormat);
    ini.beginGroup("Recent");
    const QStringList files = ini.value(identifier).toStringList();
    ini.endGroup();

    for (const QString &file : files) {
        recent.append(RecentFile(file, thumbnailPath(file)));
    }
}

bool RecentList::add(const QString &filename, QPixmap thumbnail)
{
    if (filename.isEmpty()) {
        return false;
    }

    if (thumbnail.isNull()) {
        thumbnail = iconProvider.icon(filename).pixmap(Utils::scale(32, 32));
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

bool RecentList::remove(int index)
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

void RecentList::clear()
{
    while (!recent.isEmpty()) {
        remove(0);
    }
}

void RecentList::setLimit(int value)
{
    limit = value;
    while (recent.size() > limit) {
        remove(recent.size() - 1);
    }
}

const QList<RecentFile> &RecentList::all() const
{
    return recent;
}

QStringList RecentList::filenames() const
{
    QStringList result;
    for (const RecentFile &entry : recent) {
        result << entry.filename();
    }
    return result;
}

QList<QPixmap> RecentList::thumbnails() const
{
    QList<QPixmap> result;
    for (const RecentFile &entry : recent) {
        result << entry.thumbnail();
    }
    return result;
}

void RecentList::saveToFile() const
{
    QSettings ini(Utils::getLocalConfigPath("recent/recent.ini"), QSettings::IniFormat);
    ini.beginGroup("Recent");
    ini.setValue(identifier, filenames());
    ini.endGroup();
}

QString RecentList::thumbnailPath(const QString &filename) const
{
    return thumbsPath + hash(filename) + ".png";
}

QString RecentList::hash(const QString &string) const
{
    return QCryptographicHash::hash(string.toUtf8(), QCryptographicHash::Md5).toHex();
}
