#include "base/recent.h"
#include "base/application.h"
#include "base/debug.h"
#include <QCryptographicHash>
#include <QSettings>
#include <QDir>

Recent::Recent(const QString &identifier, QObject *parent) : QObject(parent)
{
    this->identifier = identifier;
    this->thumbsPath = QString("%1/%2/").arg(app->getLocalConfigPath("recent/thumbnails"), identifier);
    this->limit = app->settings->getRecentLimit();

    QSettings ini(app->getLocalConfigPath("recent/recent.ini"), QSettings::IniFormat);
    ini.beginGroup("Recent");
    QStringList files = ini.value(identifier).toStringList();
    ini.endGroup();

    foreach (const QString &file, files) {
        recent.append(QSharedPointer<RecentFile>(new RecentFile(file, thumbnailPath(file))));
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

    for (int i = 0; i < recent.size(); ++i) {
        if (recent[i]->filename == filename) {
            recent.removeAt(i);
        }
    }

    // Create recent entry:

    recent.prepend(QSharedPointer<RecentFile>(new RecentFile(filename, thumbnail)));
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
    const QString filename = recent[index]->filename;
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

const QList<QSharedPointer<RecentFile> > &Recent::all() const
{
    return recent;
}

QStringList Recent::filenames() const
{
    QStringList result;
    foreach (const QSharedPointer<RecentFile> &entry, recent) {
        result << entry->filename;
    }
    return result;
}

QList<QPixmap> Recent::thumbnails() const
{
    QList<QPixmap> result;
    foreach (const QSharedPointer<RecentFile> &entry, recent) {
        result << entry->thumbnail;
    }
    return result;
}

void Recent::saveToFile() const
{
    QSettings ini(app->getLocalConfigPath("recent/recent.ini"), QSettings::IniFormat);
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
