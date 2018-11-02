#include "apk/manifest.h"
#include <QTextStream>
#include <QDebug>

Manifest::Manifest(const QString &xmlPath, const QString &ymlPath)
{
    // XML:

    xmlFile = new QFile(xmlPath);
    if (xmlFile->open(QFile::ReadWrite | QFile::Text)) {
        QTextStream stream(xmlFile);
        stream.setCodec("UTF-8");
        xml.setContent(stream.readAll());
        applicationLabel = getXmlAttribute({"application", "android:label"});
        applicationIcon = getXmlAttribute({"application", "android:icon"});
#ifdef QT_DEBUG
        qDebug() << "Parsed app label:   " << applicationLabel.value();
        qDebug() << "Parsed app icon:    " << applicationIcon.value();
#endif
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

    ymlFile = new QFile(ymlPath);
    if (ymlFile->open(QFile::ReadWrite | QFile::Text)) {
        QTextStream stream(ymlFile);
        stream.setCodec("UTF-8");
        yml = stream.readAll();
        minSdk = regexMinSdk.match(yml).captured().toInt();
        targetSdk = regexTargetSdk.match(yml).captured().toInt();
        versionCode = regexVersionCode.match(yml).captured().toInt();
        versionName = regexVersionName.match(yml).captured();
#ifdef QT_DEBUG
        qDebug() << "Parsed target SDK:  " << targetSdk;
        qDebug() << "Parsed minimum SDK: " << minSdk;
        qDebug() << "Parsed version code:" << versionCode;
        qDebug() << "Parsed version name:" << versionName;
#endif
    }
}

Manifest::~Manifest()
{
    delete xmlFile;
    delete ymlFile;
}

QDomAttr Manifest::getXmlAttribute(QStringList tree) const
{
    if (!tree.isEmpty()) {
        const QString attribute = tree.takeLast();
        QDomElement node = xml.firstChildElement("manifest");
        for (const QString &element : tree) {
            node = node.firstChildElement(element);
        }
        return node.attributeNode(attribute);
    }
    return QDomAttr();
}

QString Manifest::getApplicationLabel() const
{
    return applicationLabel.value();
//    QDomDocument dom;
//    dom.setContent(manifest);
//    return dom.firstChildElement("manifest")
//              .firstChildElement("application")
//              .attribute("android:label");
}

QString Manifest::getApplicationIcon() const
{
    return applicationIcon.value();
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

QString Manifest::getVersionName() const
{
    return versionName;
}

void Manifest::setApplicationLabel(const QString &value)
{
    applicationLabel.setValue(value);
    saveXml();
}

void Manifest::setApplicationIcon(const QString &value)
{
    applicationIcon.setValue(value);
    saveXml();
}

void Manifest::setMinSdk(int value)
{
    value = qMax(0, value);
    minSdk = value;
    yml.replace(regexMinSdk, QString::number(value));
    saveYml();
}

void Manifest::setTargetSdk(int value)
{
    value = qMax(1, value);
    targetSdk = value;
    yml.replace(regexTargetSdk, QString::number(value));
    saveYml();
}

void Manifest::setVersionCode(int value)
{
    value = qMax(0, value);
    versionCode = value;
    yml.replace(regexVersionCode, QString::number(value));
    saveYml();
}

void Manifest::setVersionName(const QString &value)
{
    versionName = value;
    yml.replace(regexVersionName, value);
    saveYml();
}

bool Manifest::saveXml()
{
    if (xmlFile->isWritable()) {
        xmlFile->resize(0);
        QTextStream stream(xmlFile);
        stream.setCodec("UTF-8");
        xml.save(stream, 4);
        return true;
    } else {
        qWarning() << "Error: Could not save AndroidManifest.xml";
        return false;
    }
}

bool Manifest::saveYml()
{
    if (ymlFile->isWritable()) {
        ymlFile->resize(0);
        QTextStream stream(ymlFile);
        stream.setCodec("UTF-8");
        stream << yml;
        return true;
    } else {
        qWarning() << "Error: Could not save apktool.yml";
        return false;
    }
}
