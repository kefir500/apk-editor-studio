#ifndef RECENT_H
#define RECENT_H

#include <QPixmap>
#include <QDebug>

struct RecentFile {
    RecentFile(const QString &filename, const QPixmap &thumbnail) : filename(filename), thumbnail(thumbnail) {}
    const QString filename;
    const QPixmap thumbnail;
};

class Recent : public QObject
{
    Q_OBJECT

public:
    Recent(const QString &identifier, QObject *parent = 0);

    bool add(const QString &filename, const QPixmap &thumbnail);
    bool remove(int index);
    void clear();

    void setLimit(int limit);

    const QList<RecentFile> &all() const;
    QStringList filenames() const;
    QList<QPixmap> thumbnails() const;

signals:
    void changed() const;

private:
    void saveToFile() const;
    QString thumbnailPath(const QString &filename) const;
    QString hash(const QString &string) const;

    QList<RecentFile> recent;
    QString identifier;
    QString thumbsPath;
    int limit;
};

#endif // RECENT_H
