#ifndef FILEFORMAT_H
#define FILEFORMAT_H

#include <QMimeType>

class FileFormat
{
public:
    static FileFormat fromMime(const QMimeType &mime, const QString &title = QString());
    static FileFormat fromFilename(const QString &filename, const QString &title = QString());
    static FileFormat fromExtension(const QString &extension, const QString &title = QString());

    void addExtension(const QString &extension);
    bool hasExtension(const QString &extension) const;
    const QStringList &getExtensions() const;
    QString getFilterString() const;
    bool isValid() const;

private:
    FileFormat() : valid(false) {}

    QString title;
    QStringList extensions;
    bool valid;
};

#endif // FILEFORMAT_H
