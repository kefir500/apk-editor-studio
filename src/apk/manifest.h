#ifndef MANIFEST_H
#define MANIFEST_H

#include <QFile>
#include <QDomDocument>
#include <QRegularExpression>

class Manifest
{
public:
    Manifest(const QString &xmlPath, const QString &ymlPath);
    ~Manifest();

    QDomAttr getXmlAttribute(QStringList tree) const;

    QString getApplicationLabel() const;
    QString getApplicationIcon() const;
    int getMinSdk() const;
    int getTargetSdk() const;
    int getVersionCode() const;
    QString getVersionName() const;

    void setApplicationLabel(const QString &value);
    void setApplicationIcon(const QString &value);
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

    QDomAttr applicationLabel;
    QDomAttr applicationIcon;

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
