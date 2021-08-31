#ifndef RECENTFILE_H
#define RECENTFILE_H

#include <QPixmap>

class RecentFile
{
public:
    RecentFile(const QString &filename, const QPixmap &thumbnail);
    const QString &filename() const;
    const QPixmap &thumbnail() const;
    bool operator==(const RecentFile &) const;

private:
    class RecentFilePrivate : public QSharedData
    {
    public:
        RecentFilePrivate(const QString &filename, const QPixmap &thumbnail) : filename(filename), thumbnail(thumbnail) {}
        const QString filename;
        const QPixmap thumbnail;
    };

    QSharedDataPointer<RecentFilePrivate> d;
};

#endif // RECENTFILE_H
