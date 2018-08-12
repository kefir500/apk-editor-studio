#include "base/formats.h"

QStringList Formats::extensionsApk() const
{
    return QStringList() << "apk";
}

QStringList Formats::extensionsPng() const
{
    return QStringList() << "png";
}

QStringList Formats::extensionsJpeg() const
{
    return QStringList() << "jpg"
                         << "jpeg";
}

QStringList Formats::extensionsGif() const
{
    return QStringList() << "gif";
}

QStringList Formats::extensionsImages() const
{
    return QStringList() << extensionsPng()
                         << extensionsJpeg()
                         << extensionsGif();
}

QStringList Formats::extensionsXml() const
{
    return QStringList() << "xml"
                         << "htm"
                         << "html";
}

QStringList Formats::extensionsYaml() const
{
    return QStringList() << "yml"
                         << "yaml";
}

QStringList Formats::extensionsCode() const
{
    return QStringList() << extensionsXml()
                         << extensionsYaml()
                         << "smali";
}

QStringList Formats::extensionsKeystore() const
{
    return QStringList() << "jks"
                         << "keystore";
}

QString Formats::filterApk() const
{
    QString filter;
    filter.append(constructFilter(("APK"), extensionsApk()));
    filter.append(filterAllFiles());
    return filter;
}

QString Formats::filterImages() const
{
    QString filter;
    filter.append(constructFilter("PNG", extensionsPng()));
    filter.append(constructFilter("JPEG", extensionsJpeg()));
    filter.append(constructFilter("GIF", extensionsGif()));
    filter.append(constructFilter(tr("All supported images"), extensionsImages()));
    filter.append(filterAllFiles());
    return filter;
}

QString Formats::filterKeystore() const
{
    QString filter;
    filter.append(constructFilter(tr("Keystore"), extensionsKeystore()));
    filter.append(filterAllFiles());
    return filter;
}

QString Formats::filterAllFiles() const
{
    QString filter = constructFilter(tr("All files"), "*");
    filter.chop(2);
    return filter;
}

QString Formats::filterExtension(const QString &extension) const
{
    QString filter;
    filter.append(constructFilter(extension.toUpper(), extension));
    filter.append(filterAllFiles());
    return filter;
}

QString Formats::constructFilter(const QString &title, const QString &extension) const
{
    return constructFilter(title, QStringList() << extension);
}

QString Formats::constructFilter(const QString &title, const QStringList &extensions) const
{
    QString result = title + " (";
    foreach (const QString extension, extensions) {
        if (extension.contains('*')) {
            result.append(QString("%1 ").arg(extension));
        } else {
            result.append(QString("*.%1 ").arg(extension));
        }
    }
    result.chop(1);
    result.append(");;");
    return result;
}
