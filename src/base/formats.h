#ifndef FORMATS_H
#define FORMATS_H

#include <QCoreApplication>
#include <QStringList>

class Formats
{
    Q_DECLARE_TR_FUNCTIONS(Formats)

public:
     bool isImage(const QString &format) const;
     bool isCode(const QString &format) const;

    QStringList extensionsApk() const;

    QStringList extensionsPng() const;
    QStringList extensionsJpeg() const;
    QStringList extensionsBmp() const;
    QStringList extensionsGif() const;
    QStringList extensionsImages() const;

    QStringList extensionsXml() const;
    QStringList extensionsYaml() const;
    QStringList extensionsCode() const;

    QStringList extensionsKeystore() const;

    QString filterApk() const;
    QString filterImages() const;
    QString filterKeystore() const;
    QString filterAllFiles() const;
    QString filterExtension(const QString &extension) const;

private:
    QString constructFilter(const QString &title, const QString &extension) const;
    QString constructFilter(const QString &title, const QStringList &extensions) const;
};

#endif // FORMATS_H
