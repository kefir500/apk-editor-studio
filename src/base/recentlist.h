#ifndef RECENTLIST_H
#define RECENTLIST_H

#include "base/recentfile.h"

class RecentList : public QObject
{
public:
    RecentList(const QString &identifier, int limit = 10, QObject *parent = nullptr);

    bool add(const QString &filename, const QPixmap &thumbnail);
    bool remove(int index);
    void clear();

    void setLimit(int limit);

    const QList<RecentFile> &all() const;
    QStringList filenames() const;
    QList<QPixmap> thumbnails() const;

private:
    void saveToFile() const;
    QString thumbnailPath(const QString &filename) const;
    QString hash(const QString &string) const;

    QList<RecentFile> recent;
    QString identifier;
    QString thumbsPath;
    int limit;
};

#endif // RECENTLIST_H
