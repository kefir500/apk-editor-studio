#ifndef MANIFEST_H
#define MANIFEST_H

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

    bool setApplicationLabel(const QString &value);
    bool setMinSdk(int value);
    bool setTargetSdk(int value);
    bool setVersionCode(int value);
    bool setVersionName(const QString &value);
    bool setPackageName(const QString &newPackageName);

    QList<Permission> getPermissionList() const;
    Permission addPermission(const QString &permission);
    void removePermission(const Permission &permission);

    QList<ManifestScope *> scopes;
    ManifestScope *applicationScope;

private:
    bool saveXml();
    bool saveYml();

    QString xmlPath;
    QDomDocument xmlDom;

    QDomElement manifestNode;

    QString ymlPath;
    QString ymlContents;

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
