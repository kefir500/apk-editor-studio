#include "apk/manifest.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>


Manifest::Manifest(const QString &xmlPath, const QString &ymlPath)
    : xmlPath(xmlPath)
    , ymlPath(ymlPath)
{
    // XML:

    QFile xmlFile(xmlPath);
    if (xmlFile.open(QFile::ReadOnly)) {
        QTextStream stream(&xmlFile);
        stream.setCodec("UTF-8");
        xmlDom.setContent(stream.readAll());
        manifestNode = xmlDom.firstChildElement("manifest");
        auto applicationNode = manifestNode.firstChildElement("application");
        applicationScope = new ManifestScope(applicationNode);
        scopes.append(applicationScope);
        auto applicationChild = applicationNode.firstChildElement();
        while (!applicationChild.isNull()) {
            if (QStringList({"application", "activity"}).contains(applicationChild.nodeName())) {
                if (applicationChild.hasAttribute("android:icon") ||
                    applicationChild.hasAttribute("android:roundIcon") ||
                    applicationChild.hasAttribute("android:banner")) {
                        scopes.append(new ManifestScope(applicationChild));
                }
            }
            applicationChild = applicationChild.nextSiblingElement();
        }
        packageName = manifestNode.attribute("package");
        xmlFile.close();
    } else {
        qWarning() << "Error: Could not read AndroidManifest.xml";
    }

    // YAML:

    regexMinSdk.setPatternOptions(QRegularExpression::MultilineOption);
    regexTargetSdk.setPatternOptions(QRegularExpression::MultilineOption);
    regexVersionCode.setPatternOptions(QRegularExpression::MultilineOption);
    regexVersionName.setPatternOptions(QRegularExpression::MultilineOption);
    regexMinSdk.setPattern("(?<=^  minSdkVersion: ')\\d+(?='$)");
    regexTargetSdk.setPattern("(?<=^  targetSdkVersion: ')\\d+(?='$)");
    regexVersionCode.setPattern("(?<=^  versionCode: ')\\d+(?='$)");
    regexVersionName.setPattern("(?<=^  versionName: ).+(?=$)");

    QFile ymlFile(ymlPath);
    if (ymlFile.open(QFile::ReadOnly)) {
        QTextStream stream(&ymlFile);
        stream.setCodec("UTF-8");
        ymlContents = stream.readAll();
        minSdk = regexMinSdk.match(ymlContents).captured().toInt();
        targetSdk = regexTargetSdk.match(ymlContents).captured().toInt();
        versionCode = regexVersionCode.match(ymlContents).captured().toInt();
        versionName = regexVersionName.match(ymlContents).captured();
        ymlFile.close();
    } else {
        qWarning() << "Error: Could not read apktool.yml";
    }
}

Manifest::~Manifest()
{
    qDeleteAll(scopes);
}

int Manifest::getMinSdk() const
{
    return minSdk;
}

int Manifest::getTargetSdk() const
{
    return targetSdk;
}

int Manifest::getVersionCode() const
{
    return versionCode;
}

const QString &Manifest::getVersionName() const
{
    return versionName;
}

const QString &Manifest::getPackageName() const
{
    return packageName;
}

bool Manifest::setApplicationLabel(const QString &value)
{
    auto label = applicationScope->label();
    label.setValue(value);
    return saveXml();
}

bool Manifest::setMinSdk(int value)
{
    value = qMax(0, value);
    minSdk = value;
    ymlContents.replace(regexMinSdk, QString::number(value));
    return saveYml();
}

bool Manifest::setTargetSdk(int value)
{
    value = qMax(1, value);
    targetSdk = value;
    ymlContents.replace(regexTargetSdk, QString::number(value));
    return saveYml();
}

bool Manifest::setVersionCode(int value)
{
    value = qMax(0, value);
    versionCode = value;
    ymlContents.replace(regexVersionCode, QString::number(value));
    return saveYml();
}

bool Manifest::setVersionName(const QString &value)
{
    versionName = value;
    ymlContents.replace(regexVersionName, value);
    return saveYml();
}

bool Manifest::setPackageName(const QString &newPackageName)
{
    const auto originalPackageName = getPackageName();
    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::ReadWrite)) {
        qWarning() << "Error: Could not save AndroidManifest.xml";
        return false;
    }
    xmlFile.seek(0);
    const QString xmlContents(xmlFile.readAll());
    QString newContents(xmlContents);
    newContents.replace(originalPackageName, newPackageName);
    if (newContents != xmlContents) {
        xmlFile.resize(0);
        xmlFile.write(newContents.toUtf8());
        xmlFile.flush();
    }
    packageName = newPackageName;
    return true;
}

QList<Permission> Manifest::getPermissionList() const
{
    QList<Permission> permissions;
    const auto manifestChildNodes = manifestNode.childNodes();
    const int nodeCount = manifestChildNodes.count();
    permissions.reserve(nodeCount);
    for (int i = 0; i < nodeCount; ++i) {
        const auto element = manifestChildNodes.at(i).toElement();
        if (element.tagName() == "uses-permission") {
            permissions << Permission(element);
        }
    }
    return permissions;
}

Permission Manifest::addPermission(const QString &permission)
{
    auto element = xmlDom.createElement("uses-permission");
    element.setAttribute("android:name", permission);
    manifestNode.appendChild(element);
    saveXml();
    return Permission(element);
}

void Manifest::removePermission(const Permission &permission)
{
    manifestNode.removeChild(permission.getNode());
    saveXml();
}

bool Manifest::saveXml()
{
    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::WriteOnly)) {
        qWarning() << "Error: Could not save AndroidManifest.xml";
        return false;
    }
    xmlFile.resize(0);
    QTextStream stream(&xmlFile);
    xmlDom.save(stream, 4);
    return true;
}

bool Manifest::saveYml()
{
    QFile ymlFile(ymlPath);
    if (!ymlFile.open(QFile::WriteOnly)) {
        qWarning() << "Error: Could not save apktool.yml";
        return false;
    }
    ymlFile.resize(0);
    QTextStream stream(&ymlFile);
    stream.setCodec("UTF-8");
    stream << ymlContents;
    return true;
}
