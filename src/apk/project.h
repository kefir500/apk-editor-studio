#ifndef PROJECT_H
#define PROJECT_H

#include "apk/resourcesmodel.h"
#include "apk/manifestmodel.h"
#include "apk/iconsproxy.h"
#include "apk/logmodel.h"
#include "apk/projectstate.h"
#include "base/tasks.h"
#include <QIcon>
#include <QFileSystemModel>

class Project : public QObject
{
    Q_OBJECT

public:
    Project(const QString &path);
    ~Project() Q_DECL_OVERRIDE;

    void unpack();
    void pack(QString path);
    void install(const QString &serial);
    void saveAndInstall(const QString &path, const QString &serial);

    Manifest *initialize();

    const QString &getTitle() const;
    QString getOriginalPath() const;
    QString getContentsPath() const;
    const Manifest *getManifest() const;
    QIcon getThumbnail() const;
    const ProjectState &getState() const;

    void journal(const QString &brief, LogEntry::Type type = LogEntry::Information);
    void journal(const QString &brief, const QString &descriptive, LogEntry::Type type = LogEntry::Information);

    ResourcesModel resourcesModel;
    QFileSystemModel filesystemModel;
    IconsProxy iconsProxy;
    ManifestModel manifestModel;
    LogModel logModel;

signals:
    void unpacked(bool success) const;
    void packed(bool success) const;
    void installed(bool success) const;
    void changed() const;

private:
    Tasks::Task *createUnpackTask(const QString &source);
    Tasks::Task *createSaveTask(const QString &target); // Combines Pack, Zipalign and Sign tasks
    Tasks::Task *createPackTask(const QString &target);
    Tasks::Task *createZipalignTask(const QString &target);
    Tasks::Task *createSignTask(const QString &target, const Keystore *keystore);
    Tasks::Task *createInstallTask(const QString &serial);

    const Keystore *getKeystore() const;

    ProjectState state;

    QString title;
    QString originalPath;
    QString contentsPath;
    QIcon thumbnail;
    Manifest *manifest;
};

#endif // PROJECT_H
