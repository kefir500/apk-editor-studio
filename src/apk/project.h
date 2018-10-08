#ifndef PROJECT_H
#define PROJECT_H

#include "apk/resourcesmodel.h"
#include "apk/manifestmodel.h"
#include "apk/iconsproxy.h"
#include "apk/logmodel.h"
#include "base/tasks.h"
#include <QIcon>
#include <QFuture>
#include <QFileSystemModel>

class Project : public QObject
{
    Q_OBJECT

public:
    enum State {
        ProjectNone,
        ProjectReady,
        ProjectUnpacking,
        ProjectPacking,
        ProjectSigning,
        ProjectOptimizing,
        ProjectInstalling
    };

    Project(const QString &path);
    ~Project() Q_DECL_OVERRIDE;

    void unpack();
    void pack(const QString &path);
    void install(const QString &serial);
    void saveAndInstall(const QString &path, const QString &serial);

    Manifest *initialize();

    const QString &getTitle() const;
    const QString &getOriginalPath() const;
    const QString &getContentsPath() const;
    const QIcon &getThumbnail() const;
    const Manifest *getManifest() const;

    State getState() const;
    bool getErroredState() const;
    bool getModifiedState() const;
    void setModified(bool modified);

    void journal(const QString &brief, LogEntry::Type type = LogEntry::Information);
    void journal(const QString &brief, const QString &descriptive, LogEntry::Type type = LogEntry::Information);

    ResourcesModel resourcesModel;
    ManifestModel manifestModel;
    QFileSystemModel filesystemModel;
    IconsProxy iconsProxy;
    LogModel logModel;

signals:
    void unpacked(bool success) const;
    void packed(bool success) const;
    void installed(bool success) const;
    void dataChanged() const;

private:
    Tasks::Task *createUnpackTask(const QString &source);
    Tasks::Task *createSaveTask(const QString &target); // Combines Pack, Zipalign and Sign tasks
    Tasks::Task *createPackTask(const QString &target);
    Tasks::Task *createZipalignTask(const QString &target);
    Tasks::Task *createSignTask(const QString &target, const Keystore *keystore);
    Tasks::Task *createInstallTask(const QString &serial);

    const Keystore *getKeystore() const;

    void setState(State state);
    void setErrored(bool isErrored);

    State state;
    bool isErrored;
    bool isUnpacked;
    bool isModified;

    QString title;
    QString originalPath;
    QString contentsPath;
    QIcon thumbnail;
    Manifest *manifest;
};

#endif // PROJECT_H
