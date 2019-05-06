#ifndef MANIFEST_H
#define MANIFEST_H

#include <QFile>
#include <QDomDocument>
#include <QRegularExpression>
#include "apk/manifestscope.h"

class Manifest
{
public:
    Manifest(const QString &xmlPath, const QString &ymlPath);
    ~Manifest();

    int getMinSdk() const;
    int getTargetSdk() const;
    int getVersionCode() const;
    QString getVersionName() const;

    void setApplicationLabel(const QString &value);
    void setMinSdk(int value);
    void setTargetSdk(int value);
    void setVersionCode(int value);
    void setVersionName(const QString &value);

    QList<ManifestScope *> scopes;

private:
    bool saveXml();
    bool saveYml();

    QFile *xmlFile;
    QFile *ymlFile;

    QDomDocument xml;
    QString yml;

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
