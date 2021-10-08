#ifndef SETTINGS_H
#define SETTINGS_H

#include "base/recentlist.h"

class Package;
class QSettings;

class Settings : public QObject
{
    Q_OBJECT

public:
    Settings();

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
    bool getMakeDebuggable() const;
    bool getDecompileSources() const;
    bool getKeepBrokenResources() const;
    QString getDeviceAlias(const QString &serial) const;
    QString getLastDirectory() const;
    bool getSingleInstance() const;
    bool getAutoUpdates() const;
    const QList<RecentFile> &getRecentList() const;
    int getRecentLimit() const;
    QString getLanguage() const;
    QStringList getMainWindowToolbar() const;
    QByteArray getMainWindowGeometry() const;
    QByteArray getMainWindowState() const;
    QByteArray getResourceTreeHeader() const;
    QByteArray getFileSystemTreeHeader() const;
    bool getSearchCaseSensitive() const;
    bool getSearchByRegex() const;
    QString getTheme() const;
    QStringList getAndroidExplorerToolbar() const;
    QByteArray getAndroidExplorerGeometry() const;
    QByteArray getAndroidExplorerState() const;
    bool hasRememberState(const QString &identifier) const;
    bool getRememberState(const QString &identifier) const;
#ifdef Q_OS_WIN
    bool getFileAssociation() const;
    bool getExplorerOpenIntegration() const;
    bool getExplorerInstallIntegration() const;
    bool getExplorerOptimizeIntegration() const;
    bool getExplorerSignIntegration() const;
#endif

    void addToRecent(const Package *package);
    void clearRecentList();
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
    void setMakeDebuggable(bool debuggable);
    void setDecompileSources(bool smali);
    void setKeepBrokenResources(bool keepBroken);
    void setDeviceAlias(const QString &serial, const QString &alias);
    void setLastDirectory(const QString &directory);
    void setSingleInstance(bool value);
    void setAutoUpdates(bool value);
    void setRecentLimit(int limit);
    void setLanguage(const QString &locale);
    void setMainWindowToolbar(const QStringList &actions);
    void setMainWindowGeometry(const QByteArray &geometry);
    void setMainWindowState(const QByteArray &state);
    void setResourceTreeHeader(const QByteArray &state) const;
    void setFileSystemTreeHeader(const QByteArray &state) const;
    void setSearchCaseSensitive(bool enabled) const;
    void setSearchByRegex(bool enabled) const;
    void setTheme(const QString &theme);
    void setAndroidExplorerToolbar(const QStringList &actions);
    void setAndroidExplorerGeometry(const QByteArray &geometry);
    void setAndroidExplorerState(const QByteArray &state);
    void setRememberState(const QString &identifier, bool state);
    void resetRememberState(const QString &identifier);
#ifdef Q_OS_WIN
    bool setFileAssociation(bool associate);
    bool setExplorerOpenIntegration(bool integrate);
    bool setExplorerInstallIntegration(bool integrate);
    bool setExplorerOptimizeIntegration(bool integrate);
    bool setExplorerSignIntegration(bool integrate);
#endif

signals:
    void recentListUpdated();
    void resetDone();

private:
    QSettings *settings;
    RecentList *recent;
};

#endif // SETTINGS_H
