#ifndef MANIFEST_H
#define MANIFEST_H

#include <QFile>
#include <QDomDocument>
#include <QRegularExpression>
#include "apk/manifestattribute.h"

class Manifest
{
public:
    Manifest(const QString &xmlPath, const QString &ymlPath);
    ~Manifest();

    QDomAttr getXmlAttribute(QStringList tree) const;

    const ManifestAttribute &getApplicationLabel() const;
    const ManifestAttribute &getApplicationIcon() const;
    const ManifestAttribute &getApplicationRoundIcon() const;
    const ManifestAttribute &getApplicationBanner() const;
    int getMinSdk() const;
    int getTargetSdk() const;
    int getVersionCode() const;
    QString getVersionName() const;

    void setApplicationLabel(const QString &value);
    void setMinSdk(int value);
    void setTargetSdk(int value);
    void setVersionCode(int value);
    void setVersionName(const QString &value);

private:
    bool saveXml();
    bool saveYml();

    QFile *xmlFile;
    QFile *ymlFile;

    QDomDocument xml;
    QString yml;

    ManifestAttribute applicationLabel;
    ManifestAttribute applicationIcon;
    ManifestAttribute applicationRoundIcon;
    ManifestAttribute applicationBanner;

    int minSdk;
    int targetSdk;
    int versionCode;
    QString versionName;

    QRegularExpression regexMinSdk;
    QRegularExpression regexTargetSdk;
    QRegularExpression regexVersionCode;
    QRegularExpression regexVersionName;
};

#endif // MANIFEST_H
