#ifndef MANIFEST_H
#define MANIFEST_H

#include <QFile>
#include <QDomDocument>
#include <QRegularExpression>
#include "apk/manifestscope.h"
#include "apk/permission.h"

class Manifest
{
public:
    Manifest(const QString &xmlPath, const QString &ymlPath);
    ~Manifest();

    int getMinSdk() const;
    int getTargetSdk() const;
    int getVersionCode() const;
    const QString &getVersionName() const;
    const QString &getPackageName() const;

    void setApplicationLabel(const QString &value);
    void setMinSdk(int value);
    void setTargetSdk(int value);
    void setVersionCode(int value);
    void setVersionName(const QString &value);
    void setPackageName(const QString &newPackageName);

    QList<Permission> getPermissionList() const;
    Permission addPermission(const QString &permission);
    void removePermission(const Permission &permission);

    QList<ManifestScope *> scopes;

private:
    bool saveXml();
    bool saveYml();

    QFile *xmlFile;
    QDomDocument xml;
    QDomElement manifestNode;

    QString yml;
    QFile *ymlFile;

    int minSdk;
    int targetSdk;
    int versionCode;
    QString versionName;
    QString packageName;

    QRegularExpression regexMinSdk;
    QRegularExpression regexTargetSdk;
    QRegularExpression regexVersionCode;
    QRegularExpression regexVersionName;
};

#endif // MANIFEST_H
