#ifndef RESOURCEFILE_H
#define RESOURCEFILE_H

#include <QString>
#include <QPixmap>

class ResourceFile
{
public:
    ResourceFile(const QString &path);

    QString getQualifiers() const;
    QString getReadableQualifiers() const;
    QString getName() const;
    QString getType() const; // E.g., "drawable", "values", etc.
    QString getDpi() const;
    QString getLocaleCode() const;
    QString getLanguageName() const;
    QPixmap getLanguageIcon() const;
    QString getApiVersion() const;
    QString getFileName() const;
    QString getFilePath() const;
    QString getDirectory() const;
    QIcon getFileIcon() const;

private:
    QString path;
    QString type;
    QString qualifiers;
    QString readableQualifiers;

    // Qualifiers:
    QString locale;
    QString localeLegacy;
    QString layoutDirection;
    QString apiVersion;
    QString smallestWidth;
    QString availableWidth;
    QString availableHeight;
    QString screenSize;
    QString screenAspect;
    QString roundScreen;
    QString wideColorGamut;
    QString hdr;
    QString screenOrientation;
    QString uiMode;
    QString nightMode;
    QString dpi;
    QString touchscreenType;
    QString keyboardAvailability;
    QString inputMethod;
    QString navigationAvailability;
    QString navigationMethod;
};

#endif // RESOURCEFILE_H
