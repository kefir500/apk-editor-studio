#include "base/settings.h"
#include "base/application.h"
#include "base/password.h"
#include "tools/apktool.h"
#include <QMessageBox>

Settings::Settings()
{
#ifndef PORTABLE
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, app->getTitleNoSpaces(), "config");
#else
    settings = new QSettings(app->getLocalConfigPath("config/config.ini"), QSettings::IniFormat);
#endif
}

Settings::~Settings()
{
    delete settings;
}

bool Settings::reset(QWidget *parent)
{
    const int answer = QMessageBox::question(parent, QString(), app->translate("Settings", "Are you sure you want to reset settings?"));
    if (answer == QMessageBox::Yes) {
        Apktool::reset();
        settings->clear();
        app->recent->clear();
        QDir().mkpath(Apktool::getOutputPath());
        QDir().mkpath(Apktool::getFrameworksPath());
        Password passwordKeystore("keystore");
        Password passwordKey("key");
        passwordKeystore.remove();
        passwordKey.remove();
        return true;
    }
    return false;
}

// Getters:

QString Settings::getJavaPath()
{
    return settings->value("Preferences/Java").toString();
}

QString Settings::getApktoolPath()
{
    return settings->value("Apktool/Path").toString();
}

QString Settings::getOutputDirectory()
{
    return settings->value("Apktool/Output").toString();
}

QString Settings::getFrameworksDirectory()
{
    return settings->value("Apktool/Frameworks").toString();
}

bool Settings::getSignApk()
{
    return settings->value("Signer/Enabled", true).toBool();
}

bool Settings::getOptimizeApk()
{
    return settings->value("Zipalign/Enabled", true).toBool();
}

QString Settings::getApksignerPath()
{
    return settings->value("Signer/Path").toString();
}

QString Settings::getZipalignPath()
{
    return settings->value("Zipalign/Path").toString();
}

QString Settings::getAdbPath()
{
    return settings->value("ADB/Path").toString();
}

bool Settings::getCustomKeystore()
{
    return !settings->value("Signer/DemoKey", true).toBool();
}

QString Settings::getKeystorePath()
{
    return settings->value("Signer/Keystore").toString();
}

QString Settings::getKeystorePassword()
{
    Password password("APK Editor Studio - Keystore");
    return password.get();
}

QString Settings::getKeyAlias()
{
    return settings->value("Signer/Alias").toString();
}

QString Settings::getKeyPassword()
{
    Password password("APK Editor Studio - Key");
    return password.get();
}

QString Settings::getApktoolVersion()
{
    return settings->value("Apktool/Version").toString();
}

bool Settings::getDecompileSources()
{
    return settings->value("Apktool/Sources", false).toBool();
}

bool Settings::getKeepBrokenResources()
{
    return settings->value("Apktool/KeepBroken", false).toBool();
}

QString Settings::getDeviceAlias(const QString &serial)
{
    return settings->value(QString("Devices/%1").arg(serial)).toString();
}

QString Settings::getLastDirectory()
{
    return settings->value("Preferences/LastDirectory").toString();
}

bool Settings::getAutoUpdates()
{
    return settings->value("Preferences/AutoUpdates", true).toBool();
}

int Settings::getRecentLimit()
{
    return settings->value("Preferences/MaxRecent", 10).toInt();
}

QString Settings::getLanguage()
{
    return settings->value("Preferences/Language", "en").toString();
}

QStringList Settings::getToolbar()
{
    QStringList defaults;
    defaults << "open-project" << "save-project" << "install-project" << "separator"
             << "save" << "save-as" << "separator"
             << "project-manager" << "separator"
             << "close-project" << "separator"
             << "settings" << "spacer" << "donate";
    return settings->value("MainWindow/Toolbar", defaults).toStringList();
}

QByteArray Settings::getMainWindowGeometry()
{
    return settings->value("MainWindow/Geometry").toByteArray();
}

QByteArray Settings::getMainWindowState()
{
    return settings->value("MainWindow/State").toByteArray();
}

bool Settings::hasRememberState(const QString &identifier)
{
    return settings->contains(QString("Remember/%1").arg(identifier));
}

bool Settings::getRememberState(const QString &identifier)
{
    return settings->value(QString("Remember/%1").arg(identifier)).toBool();
}

// Setters:

void Settings::setJavaPath(const QString &path)
{
    settings->setValue("Preferences/Java", path);
}

void Settings::setApktoolPath(const QString &path)
{
    settings->setValue("Apktool/Path", path);
}

void Settings::setOutputDirectory(const QString &directory)
{
    settings->setValue("Apktool/Output", directory);
}

void Settings::setFrameworksDirectory(const QString &directory)
{
    settings->setValue("Apktool/Frameworks", directory);
}

void Settings::setSignApk(bool sign)
{
    settings->setValue("Signer/Enabled", sign);
}

void Settings::setOptimizeApk(bool sign)
{
    settings->setValue("Zipalign/Enabled", sign);
}

void Settings::setApksignerPath(const QString &path)
{
    settings->setValue("Signer/Path", path);
}

void Settings::setZipalignPath(const QString &path)
{
    settings->setValue("Zipalign/Path", path);
}

void Settings::setAdbPath(const QString &path)
{
    settings->setValue("ADB/Path", path);
}

void Settings::setCustomKeystore(bool custom)
{
    settings->setValue("Signer/DemoKey", !custom);
}

void Settings::setKeystorePath(const QString &path)
{
    settings->setValue("Signer/Keystore", path);
}

void Settings::setKeystorePassword(const QString &value)
{
    Password password("APK Editor Studio - Keystore");
    return password.set(value);
}

void Settings::setKeyAlias(const QString &alias)
{
    settings->setValue("Signer/Alias", alias);
}

void Settings::setKeyPassword(const QString &value)
{
    Password password("APK Editor Studio - Key");
    return password.set(value);
}

void Settings::setApktoolVersion(const QString &version)
{
    settings->setValue("Apktool/Version", version);
}

void Settings::setDecompileSources(bool smali)
{
    settings->setValue("Apktool/Sources", smali);
}

void Settings::setKeepBrokenResources(bool keepBroken)
{
    settings->setValue("Apktool/KeepBroken", keepBroken);
}

void Settings::setDeviceAlias(const QString &serial, const QString &alias)
{
    settings->setValue(QString("Devices/%1").arg(serial), alias);
}

void Settings::setLastDirectory(const QString &directory)
{
    settings->setValue("Preferences/LastDirectory", directory);
}

void Settings::setAutoUpdates(bool value)
{
    settings->setValue("Preferences/AutoUpdates", value);
}

void Settings::setRecentLimit(int limit)
{
    settings->setValue("Preferences/MaxRecent", limit);
}

void Settings::setLanguage(const QString &locale)
{
    settings->setValue("Preferences/Language", locale);
}

void Settings::setToolbar(const QStringList &actions)
{
    settings->setValue("MainWindow/Toolbar", actions);
}

void Settings::setMainWindowGeometry(const QByteArray &geometry)
{
    settings->setValue("MainWindow/Geometry", geometry);
}

void Settings::setMainWindowState(const QByteArray &state)
{
    settings->setValue("MainWindow/State", state);
}

void Settings::setRememberState(const QString &identifier, bool state)
{
    settings->setValue(QString("Remember/%1").arg(identifier), state);
}

void Settings::resetRememberState(const QString &identifier)
{
    settings->remove(QString("Remember/%1").arg(identifier));
}
