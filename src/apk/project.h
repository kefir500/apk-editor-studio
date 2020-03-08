#ifndef PROJECT_H
#define PROJECT_H

#include "apk/resourceitemsmodel.h"
#include "apk/manifestmodel.h"
#include "apk/filesystemmodel.h"
#include "apk/iconitemsmodel.h"
#include "apk/logmodel.h"
#include "apk/projectstate.h"
#include "base/command.h"
#include "tools/apktool.h"
#include "tools/keystore.h"
#include <QIcon>

class Project : public QObject
{
    Q_OBJECT

public:
    Project(const QString &path);
    ~Project() override;

    void unpack();
    void save(QString path);
    void install(const QString &serial);
    void saveAndInstall(QString path, const QString &serial);

    const QString &getTitle() const;
    QString getOriginalPath() const;
    QString getContentsPath() const;
    QIcon getThumbnail() const;
    const ProjectState &getState() const;

    void setApplicationIcon(const QString &path);

    void journal(const QString &brief, LogEntry::Type type = LogEntry::Information);
    void journal(const QString &brief, const QString &descriptive, LogEntry::Type type = LogEntry::Information);

    Manifest *manifest;

    ResourceItemsModel resourcesModel;
    FileSystemModel filesystemModel;
    IconItemsModel iconsProxy;
    ManifestModel manifestModel;
    LogModel logModel;

signals:
    void unpacked(bool success) const;
    void packed(bool success) const;
    void installed(bool success) const;
    void changed() const;

private:
    class ProjectCommand : public Commands {
    public:
        ProjectCommand(Project *project);
    };

    class LoadUnpackedCommand : public Command {
    public:
        LoadUnpackedCommand(Project *project) : project(project) {}
        void run() override;

    private:
        Project *project;
    };

    Command *createUnpackCommand(const QString &source);
    Command *createSaveCommand(QString target); // Combines Pack, Zipalign and Sign commands
    Command *createPackCommand(const QString &target);
    Command *createZipalignCommand(const QString &target);
    Command *createSignCommand(const QString &target, const Keystore *keystore);
    Command *createInstallCommand(const QString &serial);

    QSharedPointer<const Keystore> getKeystore() const;

    ProjectState state;

    QString title;
    QString originalPath;
    QString contentsPath;
    QIcon thumbnail;
};

#endif // PROJECT_H
