#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QMutex>
#include <QAction>

class Settings
{
public:
    Settings();
    ~Settings();

    bool reset(QWidget *parent = nullptr);

    QString getJavaPath();
    QString getJdkPath();
    QString getApktoolPath();
    QString getOutputDirectory();
    QString getDefaultOutputDirectory();
    QString getFrameworksDirectory();
    bool getSignApk();
    bool getOptimizeApk();
    QString getApksignerPath();
    QString getZipalignPath();
    QString getAdbPath();
    bool getCustomKeystore();
    QString getKeystorePath();
    QString getKeystorePassword();
    QString getKeyAlias();
    QString getKeyPassword();
    QString getApktoolVersion();
    bool getDecompileSources();
    QString getDeviceAlias(const QString &serial);
    QString getLastDirectory();
    bool getAutoUpdates();
    int getRecentLimit();
    QString getLanguage();
    QStringList getToolbar();
    QByteArray getMainWindowGeometry();
    QByteArray getMainWindowState();
    bool hasRememberState(const QString &identifier);
    bool getRememberState(const QString &identifier);

    void setJavaPath(const QString &path);
    void setJdkPath(const QString &path);
    void setApktoolPath(const QString &path);
    void setOutputDirectory(const QString &directory);
    void setFrameworksDirectory(const QString &directory);
    void setSignApk(bool sign);
    void setOptimizeApk(bool sign);
    void setApksignerPath(const QString &path);
    void setZipalignPath(const QString &path);
    void setAdbPath(const QString &path);
    void setCustomKeystore(bool custom);
    void setKeystorePath(const QString &path);
    void setKeystorePassword(const QString &password);
    void setKeyAlias(const QString &alias);
    void setKeyPassword(const QString &password);
    void setApktoolVersion(const QString &version);
    void setDecompileSources(bool smali);
    void setDeviceAlias(const QString &serial, const QString &alias);
    void setLastDirectory(const QString &directory);
    void setAutoUpdates(bool value);
    void setRecentLimit(int limit);
    void setLanguage(const QString &locale);
    void setToolbar(const QStringList &actions);
    void setMainWindowGeometry(const QByteArray &geometry);
    void setMainWindowState(const QByteArray &state);
    void setRememberState(const QString &identifier, bool state);
    void resetRememberState(const QString &identifier);

private:
    QSettings *settings;
    QMutex mutex;
};

#endif // SETTINGS_H
