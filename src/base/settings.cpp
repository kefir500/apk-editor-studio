#include "base/settings.h"
#include "base/application.h"
#include "base/debug.h"
#include "base/password.h"
#include "tools/apktool.h"
#include <QMessageBox>

Settings::Settings()
{
#ifndef PORTABLE
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, app->getTitle(), "config");
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
        Apktool apktool(app->getToolPath("apktool.jar"));
        apktool.reset();
        settings->clear();
        app->recent->clear();
        Password passwordKeystore("keystore");
        Password passwordKey("key");
        passwordKeystore.remove();
        passwordKey.remove();
        return true;
    }
    return false;
}

// Getters:

QString Settings::getApktoolPath()
{
    QMutexLocker locker(&mutex);
    return settings->value("Apktool/Path", app->getToolPath("apktool.jar")).toString();
}

QString Settings::getOutputDirectory()
{
    QMutexLocker locker(&mutex);
    return settings->value("Apktool/Output", app->getOutputPath()).toString();
}

QString Settings::getFrameworksDirectory()
{
    QMutexLocker locker(&mutex);
    return settings->value("Apktool/Frameworks", app->getFrameworksPath()).toString();
}

bool Settings::getSignApk()
{
    QMutexLocker locker(&mutex);
    return settings->value("Signer/Enabled", true).toBool();
}

bool Settings::getOptimizeApk()
{
    QMutexLocker locker(&mutex);
    return settings->value("Zipalign/Enabled", true).toBool();
}

QString Settings::getApksignerPath()
{
    QMutexLocker locker(&mutex);
    return settings->value("Signer/Path", app->getToolPath("apksigner.jar")).toString();
}

QString Settings::getZipalignPath()
{
    QMutexLocker locker(&mutex);
    return settings->value("Zipalign/Path", app->getToolPath("zipalign")).toString();
}

QString Settings::getAdbPath()
{
    QMutexLocker locker(&mutex);
    return settings->value("ADB/Path", app->getToolPath("adb")).toString();
}

bool Settings::getCustomKeystore()
{
    QMutexLocker locker(&mutex);
    return !settings->value("Signer/DemoKey", true).toBool();
}

QString Settings::getKeystorePath()
{
    QMutexLocker locker(&mutex);
    return settings->value("Signer/Keystore").toString();
}

QString Settings::getKeystorePassword()
{
    Password password("APK Editor Studio - Keystore");
    return password.get();
}

QString Settings::getKeyAlias()
{
    QMutexLocker locker(&mutex);
    return settings->value("Signer/Alias").toString();
}

QString Settings::getKeyPassword()
{
    Password password("APK Editor Studio - Key");
    return password.get();
}

QString Settings::getApktoolVersion()
{
    QMutexLocker locker(&mutex);
    return settings->value("Apktool/Version").toString();
}

bool Settings::getDecompileSources()
{
    QMutexLocker locker(&mutex);
    return settings->value("Apktool/Sources", false).toBool();
}

QString Settings::getDeviceAlias(const QString &serial)
{
    QMutexLocker locker(&mutex);
    return settings->value(QString("Devices/%1").arg(serial)).toString();
}

QString Settings::getLastDirectory()
{
    QMutexLocker locker(&mutex);
    return settings->value("Preferences/LastDirectory").toString();
}

bool Settings::getAutoUpdates()
{
    QMutexLocker locker(&mutex);
    return settings->value("Preferences/AutoUpdates", true).toBool();
}

int Settings::getRecentLimit()
{
    QMutexLocker locker(&mutex);
    return settings->value("Preferences/MaxRecent", 10).toInt();
}

QString Settings::getLanguage()
{
    QMutexLocker locker(&mutex);
    return settings->value("Preferences/Language", "en").toString();
}

QStringList Settings::getToolbar()
{
    QMutexLocker locker(&mutex);
    QStringList defaults;
    defaults << "open-project" << "save-project" << "separator"
             << "install-project" << "separator"
             << "save-resource" << "separator"
             << "project-manager" << "open-contents" << "separator"
             << "settings" << "spacer" << "donate";
    return settings->value("MainWindow/Toolbar", defaults).toStringList();
}

QByteArray Settings::getMainWindowGeometry()
{
    QMutexLocker locker(&mutex);
    return settings->value("MainWindow/Geometry").toByteArray();
}

QByteArray Settings::getMainWindowState()
{
    QMutexLocker locker(&mutex);
    return settings->value("MainWindow/State").toByteArray();
}

// Setters:

void Settings::setApktoolPath(const QString &path)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Apktool/Path", path);
}

void Settings::setOutputDirectory(const QString &directory)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Apktool/Output", directory);
}

void Settings::setFrameworksDirectory(const QString &directory)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Apktool/Frameworks", directory);
}

void Settings::setSignApk(bool sign)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Signer/Enabled", sign);
}

void Settings::setOptimizeApk(bool sign)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Zipalign/Enabled", sign);
}

void Settings::setApksignerPath(const QString &path)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Signer/Path", path);
}

void Settings::setZipalignPath(const QString &path)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Zipalign/Path", path);
}

void Settings::setAdbPath(const QString &path)
{
    QMutexLocker locker(&mutex);
    settings->setValue("ADB/Path", path);
}

void Settings::setCustomKeystore(bool custom)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Signer/DemoKey", !custom);
}

void Settings::setKeystorePath(const QString &path)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Signer/Keystore", path);
}

void Settings::setKeystorePassword(const QString &value)
{
    Password password("APK Editor Studio - Keystore");
    return password.set(value);
}

void Settings::setKeyAlias(const QString &alias)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Signer/Alias", alias);
}

void Settings::setKeyPassword(const QString &value)
{
    Password password("APK Editor Studio - Key");
    return password.set(value);
}

void Settings::setApktoolVersion(const QString &version)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Apktool/Version", version);
}

void Settings::setDecompileSources(bool smali)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Apktool/Sources", smali);
}

void Settings::setDeviceAlias(const QString &serial, const QString &alias)
{
    QMutexLocker locker(&mutex);
    settings->setValue(QString("Devices/%1").arg(serial), alias);
}

void Settings::setLastDirectory(const QString &directory)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Preferences/LastDirectory", directory);
}

void Settings::setAutoUpdates(bool value)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Preferences/AutoUpdates", value);
}

void Settings::setRecentLimit(int limit)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Preferences/MaxRecent", limit);
}

void Settings::setLanguage(const QString &locale)
{
    QMutexLocker locker(&mutex);
    settings->setValue("Preferences/Language", locale);
}

void Settings::setToolbar(const QStringList &actions)
{
    QMutexLocker locker(&mutex);
    settings->setValue("MainWindow/Toolbar", actions);
}

void Settings::setMainWindowGeometry(const QByteArray &geometry)
{
    QMutexLocker locker(&mutex);
    settings->setValue("MainWindow/Geometry", geometry);
}

void Settings::setMainWindowState(const QByteArray &state)
{
    QMutexLocker locker(&mutex);
    settings->setValue("MainWindow/State", state);
}
