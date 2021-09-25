#ifndef PACKAGE_H
#define PACKAGE_H

#include "apk/filesystemmodel.h"
#include "apk/iconitemsmodel.h"
#include "apk/logmodel.h"
#include "apk/manifestmodel.h"
#include "apk/packagestate.h"
#include "apk/resourceitemsmodel.h"
#include "base/command.h"
#include <QIcon>

class Keystore;

class Package : public QObject
{
    Q_OBJECT

public:
    Package(const QString &path);
    ~Package() override;

    QString getTitle() const;
    QString getOriginalPath() const;
    QString getContentsPath() const;
    QString getPackageName() const;
    QIcon getThumbnail() const;
    const PackageState &getState() const;
    bool hasSourcesUnpacked() const;

    void setApplicationIcon(const QString &path, QWidget *parent = nullptr);
    bool setPackageName(const QString &packageName);

    Manifest *manifest;

    ResourceItemsModel resourcesModel;
    FileSystemModel filesystemModel;
    IconItemsModel iconsProxy;
    ManifestModel manifestModel;
    LogModel logModel;

    Commands *createCommandChain();
    Command *createUnpackCommand();
    Command *createPackCommand(const QString &target);
    Command *createZipalignCommand(const QString &apk = QString());
    Command *createSignCommand(const Keystore *keystore, const QString &apk = QString());
    Command *createInstallCommand(const QString &serial, const QString &apk = QString());

signals:
    void stateUpdated();

private:    
    class LoadUnpackedCommand : public Command
    {
    public:
        LoadUnpackedCommand(Package *package) : package(package) {}
        void run() override;
    private:
        Package *package;
    };

    PackageState state;

    QString originalPath;
    QString contentsPath;
    QIcon thumbnail;

    bool withSources = false;
    bool withResources = false;
    bool withBrokenResources = false;
};

#endif // PACKAGE_H
