#include "base/recentfile.h"

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
