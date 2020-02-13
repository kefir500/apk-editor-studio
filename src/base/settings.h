#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT

public:
    Settings();
    ~Settings();

    void reset();

    QString getJavaPath() const;
    int getJavaMinHeapSize() const;
    int getJavaMaxHeapSize() const;
    QString getApktoolPath() const;
    QString getOutputDirectory() const;
    QString getFrameworksDirectory() const;
    bool getSignApk() const;
    bool getOptimizeApk() const;
    QString getApksignerPath() const;
    QString getZipalignPath() const;
    QString getAdbPath() const;
    bool getCustomKeystore() const;
    QString getKeystorePath() const;
    QString getKeystorePassword() const;
    QString getKeyAlias() const;
    QString getKeyPassword() const;
    QString getApktoolVersion() const;
    bool getUseAapt2() const;
    bool getDecompileSources() const;
    bool getKeepBrokenResources() const;
    QString getDeviceAlias(const QString &serial) const;
    QString getLastDirectory() const;
    bool getAutoUpdates() const;
    int getRecentLimit() const;
    QString getLanguage() const;
    QStringList getToolbar() const;
    QByteArray getMainWindowGeometry() const;
    QByteArray getMainWindowState() const;
    bool hasRememberState(const QString &identifier) const;
    bool getRememberState(const QString &identifier) const;

    void setJavaPath(const QString &path);
    void setJavaMinHeapSize(int size);
    void setJavaMaxHeapSize(int size);
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
    void setUseAapt2(bool aapt2);
    void setDecompileSources(bool smali);
    void setKeepBrokenResources(bool keepBroken);
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

signals:
    void toolbarUpdated() const;
    void resetDone() const;

private:
    QSettings *settings;
};

#endif // SETTINGS_H
