#include "base/settings.h"
#include "base/application.h"
#include "base/fileassociation.h"
#include "base/password.h"
#include "tools/apktool.h"
#include <QDir>

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

void Settings::reset()
{
    Apktool::reset();
    settings->clear();
    app->recent->clear();
    QDir().mkpath(Apktool::getOutputPath());
    QDir().mkpath(Apktool::getFrameworksPath());
    Password passwordKeystore("keystore");
    Password passwordKey("key");
    passwordKeystore.remove();
    passwordKey.remove();
    emit resetDone();
}

// Getters:

QString Settings::getJavaPath() const
{
    const auto defaultValue = settings->value("Preferences/Java");
    return settings->value("Java/Path", defaultValue).toString();
}

int Settings::getJavaMinHeapSize() const
{
    return settings->value("Java/MinHeapSize").toInt();
}

int Settings::getJavaMaxHeapSize() const
{
    return settings->value("Java/MaxHeapSize").toInt();
}

QString Settings::getApktoolPath() const
{
    return settings->value("Apktool/Path").toString();
}

QString Settings::getOutputDirectory() const
{
    return settings->value("Apktool/Output").toString();
}

QString Settings::getFrameworksDirectory() const
{
    return settings->value("Apktool/Frameworks").toString();
}

bool Settings::getSignApk() const
{
    return settings->value("Signer/Enabled", true).toBool();
}

bool Settings::getOptimizeApk() const
{
    return settings->value("Zipalign/Enabled", true).toBool();
}

QString Settings::getApksignerPath() const
{
    return settings->value("Signer/Path").toString();
}

QString Settings::getZipalignPath() const
{
    return settings->value("Zipalign/Path").toString();
}

QString Settings::getAdbPath() const
{
    return settings->value("ADB/Path").toString();
}

bool Settings::getCustomKeystore() const
{
    return !settings->value("Signer/DemoKey", true).toBool();
}

QString Settings::getKeystorePath() const
{
    return settings->value("Signer/Keystore").toString();
}

QString Settings::getKeystorePassword() const
{
    Password password("APK Editor Studio - Keystore");
    return password.get();
}

QString Settings::getKeyAlias() const
{
    return settings->value("Signer/Alias").toString();
}

QString Settings::getKeyPassword() const
{
    Password password("APK Editor Studio - Key");
    return password.get();
}

QString Settings::getApktoolVersion() const
{
    return settings->value("Apktool/Version").toString();
}

bool Settings::getUseAapt2() const
{
    return settings->value("Apktool/Aapt2", false).toBool();
}

bool Settings::getDecompileSources() const
{
    return settings->value("Apktool/Sources", false).toBool();
}

bool Settings::getKeepBrokenResources() const
{
    return settings->value("Apktool/KeepBroken", false).toBool();
}

QString Settings::getDeviceAlias(const QString &serial) const
{
    return settings->value(QString("Devices/%1").arg(serial)).toString();
}

QString Settings::getLastDirectory() const
{
    return settings->value("Preferences/LastDirectory").toString();
}

bool Settings::getSingleInstance() const
{
    return settings->value("Preferences/SingleInstance", true).toBool();
}

bool Settings::getAutoUpdates() const
{
    return settings->value("Preferences/AutoUpdates", true).toBool();
}

int Settings::getRecentLimit() const
{
    return settings->value("Preferences/MaxRecent", 10).toInt();
}

QString Settings::getLanguage() const
{
    return settings->value("Preferences/Language", "en").toString();
}

QStringList Settings::getMainWindowToolbar() const
{
    QStringList defaults;
    defaults << "open-project" << "save-project" << "install-project" << "separator"
             << "save" << "save-as" << "separator"
             << "project-manager" << "separator"
             << "android-explorer" << "separator"
             << "close-project" << "separator"
             << "settings" << "spacer" << "donate";
    return settings->value("MainWindow/Toolbar", defaults).toStringList();
}

QByteArray Settings::getMainWindowGeometry() const
{
    return settings->value("MainWindow/Geometry").toByteArray();
}

QByteArray Settings::getMainWindowState() const
{
    return settings->value("MainWindow/State").toByteArray();
}

QStringList Settings::getAndroidExplorerToolbar() const
{
    QStringList defaults;
    defaults << "download" << "upload" << "separator"
             << "copy" << "cut" << "paste" << "rename" << "delete" << "separator"
             << "install" << "separator"
             << "screenshot";
    return settings->value("AndroidExplorer/Toolbar", defaults).toStringList();
}

bool Settings::hasRememberState(const QString &identifier) const
{
    return settings->contains(QString("Remember/%1").arg(identifier));
}

bool Settings::getRememberState(const QString &identifier) const
{
    return settings->value(QString("Remember/%1").arg(identifier)).toBool();
}

#ifdef Q_OS_WIN
bool Settings::getFileAssociation() const
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    return association.isSet();
}

bool Settings::getExplorerOpenIntegration() const
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    return association.hasVerb("Open");
}

bool Settings::getExplorerInstallIntegration() const
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    return association.hasVerb("Install APK");
}

bool Settings::getExplorerOptimizeIntegration() const
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    return association.hasVerb("Optimize APK");
}

bool Settings::getExplorerSignIntegration() const
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    return association.hasVerb("Sign APK");
}
#endif

// Setters:

void Settings::setJavaPath(const QString &path)
{
    settings->setValue("Java/Path", path);
}

void Settings::setJavaMinHeapSize(int size)
{
    settings->setValue("Java/MinHeapSize", size);
}

void Settings::setJavaMaxHeapSize(int size)
{
    settings->setValue("Java/MaxHeapSize", size);
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

void Settings::setUseAapt2(bool aapt2)
{
    settings->setValue("Apktool/Aapt2", aapt2);
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

void Settings::setSingleInstance(bool value)
{
    settings->setValue("Preferences/SingleInstance", value);
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

void Settings::setMainWindowToolbar(const QStringList &actions)
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

void Settings::setAndroidExplorerToolbar(const QStringList &actions)
{
    settings->setValue("AndroidExplorer/Toolbar", actions);
}

void Settings::setRememberState(const QString &identifier, bool state)
{
    settings->setValue(QString("Remember/%1").arg(identifier), state);
}

void Settings::resetRememberState(const QString &identifier)
{
    settings->remove(QString("Remember/%1").arg(identifier));
}

#ifdef Q_OS_WIN

bool Settings::setFileAssociation(bool associate)
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    if (associate) {
        const QString iconPath = QString("%1,0").arg(QDir::toNativeSeparators(app->applicationFilePath()));
        return association.set(iconPath, "Android Application Package");
    } else {
        return association.unset();
    }
}

bool Settings::setExplorerOpenIntegration(bool integrate)
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    if (integrate) {
        const QString executablePath = QString("\"%1\"").arg(QDir::toNativeSeparators(app->applicationFilePath()));
        const QString iconPath = executablePath;
        return association.addVerb("Open", executablePath + " \"%1\"", iconPath);
    } else {
        return association.removeVerb("Open");
    }
}

bool Settings::setExplorerInstallIntegration(bool integrate)
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    if (integrate) {
        const QString executablePath = QString("\"%1\"").arg(QDir::toNativeSeparators(app->applicationFilePath()));
        const QString iconPath = QDir::toNativeSeparators(app->getSharedPath("resources/icons/other/verb-install.ico"));
        return association.addVerb("Install APK", executablePath + " --install \"%1\"", iconPath);
    } else {
        return association.removeVerb("Install APK");
    }
}

bool Settings::setExplorerOptimizeIntegration(bool integrate)
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    if (integrate) {
        const QString executablePath = QString("\"%1\"").arg(QDir::toNativeSeparators(app->applicationFilePath()));
        const QString iconPath = QDir::toNativeSeparators(app->getSharedPath("resources/icons/other/verb-optimize.ico"));
        return association.addVerb("Optimize APK", executablePath + " --optimize \"%1\"", iconPath);
    } else {
        return association.removeVerb("Optimize APK");
    }
}

bool Settings::setExplorerSignIntegration(bool integrate)
{
    FileAssociation association("apk-editor-studio.apk", "apk");
    if (integrate) {
        const QString executablePath = QString("\"%1\"").arg(QDir::toNativeSeparators(app->applicationFilePath()));
        const QString iconPath = QDir::toNativeSeparators(app->getSharedPath("resources/icons/other/verb-sign.ico"));
        return association.addVerb("Sign APK", executablePath + " --sign \"%1\"", iconPath);
    } else {
        return association.removeVerb("Sign APK");
    }
}

#endif
