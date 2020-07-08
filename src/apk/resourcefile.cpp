#include "apk/resourcefile.h"
#include "base/application.h"
#include "base/utils.h"
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QRegularExpression>

namespace Qualifiers
{
    // Read more: https://developer.android.com/guide/topics/resources/providing-resources.html?hl=en
    const QStringList layoutDirection = {"ldrtl", "ldltr"};
    const QStringList screenSize = {"small", "normal", "large", "xlarge"};
    const QStringList screenAspect = {"long", "notlong"};
    const QStringList roundScreen = {"round", "notround"};
    const QStringList wideColorGamut = {"widecg", "nowidecg"};
    const QStringList hdr = {"highdr", "lowdr"};
    const QStringList screenOrientation = {"port", "land"};
    const QStringList uiMode = {"car", "desk", "television", "appliance", "watch", "vrheadset"};
    const QStringList nightMode = {"night", "notnight"};
    const QStringList dpi = {"ldpi", "mdpi", "hdpi", "xhdpi", "xxhdpi", "xxxhdpi", "nodpi", "tvdpi", "anydpi"};
    const QStringList touchscreenType = {"notouch", "finger"};
    const QStringList keyboardAvailability = {"keysexposed", "keyshidden", "keyssoft"};
    const QStringList inputMethod = {"nokeys", "qwerty", "12key"};
    const QStringList navigationAvailability = {"navexposed", "navhidden"};
    const QStringList navigationMethod = {"nonav", "dpad", "trackball", "wheel"};
}

ResourceFile::ResourceFile(const QString &path)
{
    if (Q_UNLIKELY(path.isEmpty())) {
        qFatal("CRITICAL: Empty path passed to resource file constructor");
    }

    this->path = QDir::fromNativeSeparators(path);
    const QStringList directoryParts = path.split('/');
    if (Q_UNLIKELY(directoryParts.size() < 2)) {
        qFatal("CRITICAL: Invalid path passed to resource file constructor");
    }
    qualifiers = directoryParts.at(directoryParts.size() - 2);
    QStringList qualifiersParts = qualifiers.split('-');
    this->type = qualifiersParts.takeFirst();
    // Replace "-r" region code prefix with "_":
    qualifiersParts = qualifiersParts.join('-').replace(QRegularExpression("(-r)(?=[A-Za-z]{2}(?=\\z|-))"), "_").split('-');
    this->readableQualifiers = qualifiersParts.join(" - ");

    for (const QString &qualifier : qualifiersParts) {
        if (Qualifiers::layoutDirection.contains(qualifier)) {
            layoutDirection = qualifier;
        } else if (qualifier.contains(QRegularExpression("sw\\d+dp"))) {
            smallestWidth = qualifier;
        } else if (qualifier.contains(QRegularExpression("w\\d+dp"))) {
            availableWidth = qualifier;
        } else if (qualifier.contains(QRegularExpression("h\\d+dp"))) {
            availableHeight = qualifier;
        } else if (Qualifiers::screenSize.contains(qualifier)) {
            screenSize = qualifier;
        } else if (Qualifiers::screenAspect.contains(qualifier)) {
            screenAspect = qualifier;
        } else if (Qualifiers::roundScreen.contains(qualifier)) {
            roundScreen = qualifier;
        } else if (Qualifiers::wideColorGamut.contains(qualifier)) {
            wideColorGamut = qualifier;
        } else if (Qualifiers::hdr.contains(qualifier)) {
            hdr = qualifier;
        } else if (Qualifiers::screenOrientation.contains(qualifier)) {
            screenOrientation = qualifier;
        } else if (Qualifiers::uiMode.contains(qualifier)) {
            uiMode = qualifier;
        } else if (Qualifiers::nightMode.contains(qualifier)) {
            nightMode = qualifier;
        } else if (Qualifiers::dpi.contains(qualifier)) {
            dpi = qualifier;
        } else if (Qualifiers::touchscreenType.contains(qualifier)) {
            touchscreenType = qualifier;
        } else if (Qualifiers::keyboardAvailability.contains(qualifier)) {
            keyboardAvailability = qualifier;
        } else if (Qualifiers::inputMethod.contains(qualifier)) {
            inputMethod = qualifier;
        } else if (Qualifiers::navigationAvailability.contains(qualifier)) {
            navigationAvailability = qualifier;
        } else if (Qualifiers::navigationMethod.contains(qualifier)) {
            navigationMethod = qualifier;
        } else if (qualifier.contains(QRegularExpression("v\\d+"))) {
            apiVersion = qualifier;
        } else {
            locale = qualifier;
            // Handle legacy locales:
            if (locale == "iw") {
                localeLegacy = "he";
            } else if (locale == "ji") {
                localeLegacy = "yi";
            } else if (locale == "in") {
                localeLegacy = "id";
            } else {
                localeLegacy = locale;
            }
        }
    }
}

QString ResourceFile::getQualifiers() const
{
    return qualifiers;
}

QString ResourceFile::getReadableQualifiers() const
{
    return readableQualifiers;
}

QString ResourceFile::getName() const
{
    return QFileInfo(path).baseName();
}

QString ResourceFile::getType() const
{
    return type;
}

QString ResourceFile::getDpi() const
{
    return dpi.toUpper();
}

QString ResourceFile::getApiVersion() const
{
    return apiVersion;
}

QString ResourceFile::getLocaleCode() const
{
    return locale;
}

QString ResourceFile::getLanguageName() const
{
    QString native = QLocale(localeLegacy).nativeLanguageName();
    return Utils::capitalize(native);
}

QPixmap ResourceFile::getLanguageIcon() const
{
    return Utils::getLocaleFlag(QLocale(localeLegacy));
}

QString ResourceFile::getFileName() const
{
    return QFileInfo(path).fileName();
}

QString ResourceFile::getFilePath() const
{
    return path;
}

QString ResourceFile::getDirectory() const
{
    return QFileInfo(path).path();
}

QIcon ResourceFile::getFileIcon() const
{
    const QString filePath = getFilePath();
    if (Utils::isImageReadable(filePath)) {
        QPixmap thumbnail(filePath);
        return thumbnail;
    }
    return app->icons.get(QFileInfo(filePath));
}
