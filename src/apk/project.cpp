#include "apk/project.h"
#include "base/application.h"
#include "base/utils.h"
#include "tools/adb.h"
#include "tools/apktool.h"
#include "tools/apksigner.h"
#include "tools/zipalign.h"
#include "windows/dialogs.h"
#include "windows/keyselector.h"
#include <QUuid>
#include <QDirIterator>
#include <QInputDialog>
#include <QDebug>

Project::Project(const QString &path) : resourcesModel(this)
{
    QFileInfo fileInfo(path);
    title = fileInfo.fileName();
    originalPath = fileInfo.absoluteFilePath();
    manifest = nullptr;
    filesystemModel.setSourceModel(&resourcesModel);
    iconsProxy.setSourceModel(&resourcesModel);
    connect(&state, &ProjectState::changed, this, &Project::changed);
}

Project::~Project()
{
    delete manifest;

    if (!contentsPath.isEmpty()) {
        qDebug() << qPrintable(QString("Removing \"%1\"...\n").arg(contentsPath));
        // Additional check to prevent accidental recursive deletion of the wrong directory:
        const bool recursive = QFile::exists(QString("%1/%2").arg(contentsPath, "AndroidManifest.xml"));
        Utils::rmdir(contentsPath, recursive);
    }
}

void Project::unpack()
{
    auto command = new ProjectCommand(this);
    command->add(createUnpackCommand(getOriginalPath()), true);
    command->run();
}

void Project::save(QString path)
{
    auto command = new ProjectCommand(this);
    command->add(createSaveCommand(path), true);
    command->run();
}

void Project::saveAndInstall(QString path, const QString &serial)
{
    auto command = new ProjectCommand(this);
    command->add(createSaveCommand(path), true);
    command->add(createInstallCommand(serial), true);
    command->run();
}

void Project::install(const QString &serial)
{
    auto command = new ProjectCommand(this);
    command->add(createInstallCommand(serial), true);
    command->run();
}

const QString &Project::getTitle() const
{
    return title;
}

QString Project::getOriginalPath() const
{
    return QDir::toNativeSeparators(originalPath);
}

QString Project::getContentsPath() const
{
    return QDir::toNativeSeparators(contentsPath);
}

QIcon Project::getThumbnail() const
{
    QIcon thumbnail = iconsProxy.getIcon();
    return !thumbnail.isNull() ? thumbnail : app->icons.get("application.png");
}

const ProjectState &Project::getState() const
{
    return state;
}

void Project::setApplicationIcon(const QString &path)
{
    iconsProxy.replaceApplicationIcons(path);
}

void Project::journal(const QString &brief, LogEntry::Type type)
{
    logModel.add(brief, QString(), type);
}

void Project::journal(const QString &brief, const QString &descriptive, LogEntry::Type type)
{
    logModel.add(brief, descriptive, type);
}

Command *Project::createUnpackCommand(const QString &source)
{
    QString target;
    do {
        const QString uuid = QUuid::createUuid().toString();
        target = QDir::toNativeSeparators(QString("%1/%2").arg(Apktool::getOutputPath(), uuid));
    } while (target.isEmpty() || QDir(target).exists());
    const QString frameworks = Apktool::getFrameworksPath();
    const bool resources = true;
    const bool sources = app->settings->getDecompileSources();
    const bool keepBroken = app->settings->getKeepBrokenResources();

    QDir().mkpath(target);
    QDir().mkpath(frameworks);

    // Be careful with the "contentsPath" variable: this directory is recursively removed in the destructor.
    this->contentsPath = target;

    auto command = new Commands(this);
    command->add(new Apktool::Decode(source, target, frameworks, resources, sources, keepBroken), true);
    command->add(new LoadUnpackedCommand(this), true);
    connect(command, &Command::started, this, [=]() {
        qDebug() << qPrintable(QString("Unpacking\n  from: %1\n    to: %2\n").arg(source, target));
        journal(tr("Unpacking APK..."));
        state.setCurrentState(ProjectState::StateUnpacking);
    });
    connect(command, &Command::finished, this, [=](bool success) {
        if (success) {
            connect(&resourcesModel, &ResourceItemsModel::dataChanged, [=] () {
                state.setModified(true);
            });
            connect(&filesystemModel, &QFileSystemModel::dataChanged, [=] () {
                state.setModified(true);
            });
            connect(&iconsProxy, &IconItemsModel::dataChanged, [=] () {
                state.setModified(true);
            });
            connect(&manifestModel, &ManifestModel::dataChanged, [=] (const QModelIndex &, const QModelIndex &, const QVector<int> &roles) {
                if (!(roles.count() == 1 && roles.contains(Qt::UserRole))) {
                    state.setModified(true);
                }
            });
        } else {
            journal(tr("Error unpacking APK."), {}, LogEntry::Error);
        }
        state.setUnpacked(success);
        emit unpacked(success);
    });
    return command;
}

Command *Project::createSaveCommand(QString target) // Combines Pack, Zipalign and Sign commands
{
    target = QDir::toNativeSeparators(target);
    QFileInfo fileInfo(target);
    const QString directory = fileInfo.absolutePath();
    app->settings->setLastDirectory(directory);

    auto command = new Commands(this);

    // Pack APK:

    command->add(createPackCommand(target), true);

    // Optimize APK:

    if (app->settings->getOptimizeApk()) {
        command->add(createZipalignCommand(target), false);
    }

    // Sign APK:

    if (app->settings->getSignApk()) {
        const Keystore *keystore = getKeystore();
        if (keystore) {
            command->add(createSignCommand(target, keystore), false);
        }
    }

    // Done:

    connect(command, &Commands::finished, this, [=]() {
        QFileInfo fileInfo(target);
        title = fileInfo.fileName();
        originalPath = target;
    });

    return command;
}

Command *Project::createPackCommand(const QString &target)
{
    const QString source = getContentsPath();
    const QString frameworks = Apktool::getFrameworksPath();
    const bool aapt2 = app->settings->getUseAapt2();

    auto apktoolBuild = new Apktool::Build(source, target, frameworks, aapt2);

    connect(apktoolBuild, &Command::started, this, [=]() {
        qDebug() << qPrintable(QString("Packing\n  from: %1\n    to: %2\n").arg(source, target));
        journal(tr("Packing APK..."));
        state.setCurrentState(ProjectState::StatePacking);
    });

    connect(apktoolBuild, &Command::finished, this, [=](bool success) {
        if (success) {
            state.setModified(false);
        } else {
            journal(tr("Error packing APK."), apktoolBuild->output(), LogEntry::Error);
        }
        emit packed(success);
    });

    return apktoolBuild;
}

Command *Project::createZipalignCommand(const QString &target)
{
    auto zipalign = new Zipalign::Align(target);

    connect(zipalign, &Command::started, this, [=]() {
        journal(tr("Optimizing APK..."));
        state.setCurrentState(ProjectState::StateOptimizing);
    });

    connect(zipalign, &Command::finished, this, [=](bool success) {
        if (!success) {
            journal(tr("Error optimizing APK."), zipalign->output(), LogEntry::Error);
        }
    });

    return zipalign;
}

Command *Project::createSignCommand(const QString &target, const Keystore *keystore)
{
    auto apksigner = new Apksigner::Sign(target, keystore);

    connect(apksigner, &Command::started, this, [=]() {
        journal(tr("Signing APK..."));
        state.setCurrentState(ProjectState::StateSigning);
    });

    connect(apksigner, &Command::finished, [=](bool success) {
        delete keystore;
        if (!success) {
            journal(tr("Error signing APK."), apksigner->output(), LogEntry::Error);
        }
    });

    return apksigner;
}

Command *Project::createInstallCommand(const QString &serial)
{
    auto command = new Adb::Install(originalPath, serial);

    connect(command, &Command::started, this, [=]() {
        journal(tr("Installing APK..."));
        state.setCurrentState(ProjectState::StateInstalling);
    });

    connect(command, &Command::finished, this, [=](bool success) {
        if (!success) {
            journal(tr("Error installing APK."), command->output(), LogEntry::Error);
        }
        emit installed(success);
    });

    return command;
}

const Keystore *Project::getKeystore() const
{
    Keystore *keystore = new Keystore;
    if (app->settings->getCustomKeystore()) {
        keystore->keystorePath = app->settings->getKeystorePath();
        keystore->keystorePassword = app->settings->getKeystorePassword();
        keystore->keyAlias = app->settings->getKeyAlias();
        keystore->keyPassword = app->settings->getKeyPassword();
        if (keystore->keystorePath.isEmpty()) {
            keystore->keystorePath = Dialogs::getOpenKeystoreFilename();
            if (keystore->keystorePath.isEmpty()) {
                return nullptr;
            }
        }
        if (keystore->keystorePassword.isEmpty()) {
            bool accepted;
            keystore->keystorePassword = QInputDialog::getText(nullptr, QString(), tr("Enter the keystore password:"), QLineEdit::Password, QString(), &accepted);
            if (!accepted) {
                return nullptr;
            }
        }
        if (keystore->keyAlias.isEmpty()) {
            KeySelector keySelector(keystore->keystorePath, keystore->keystorePassword);
            keystore->keyAlias = keySelector.select();
            if (keystore->keyAlias.isEmpty()) {
                return nullptr;
            }
        }
        if (keystore->keyPassword.isEmpty()) {
            bool accepted;
            keystore->keyPassword = QInputDialog::getText(nullptr, QString(), tr("Enter the key password:"), QLineEdit::Password, QString(), &accepted);
            if (!accepted) {
                return nullptr;
            }
        }
    } else {
        // This keystore is provided for demonstrational purposes.
        keystore->keystorePath = app->getSharedPath("tools/demo.jks");
        keystore->keystorePassword = "123456";
        keystore->keyAlias = "demo";
        keystore->keyPassword = "123456";
    }
    return keystore;
}

Project::ProjectCommand::ProjectCommand(Project *project)
{
    connect(this, &Command::started, [=]() {
        project->logModel.clear();
        project->logModel.setLoadingState(true);
    });
    connect(this, &Command::finished, this, [=](bool success) {
        project->logModel.setLoadingState(false);
        if (success) {
            project->journal(tr("Done."), LogEntry::Success);
            project->state.setCurrentState(ProjectState::StateNormal);
        } else {
            project->state.setCurrentState(ProjectState::StateErrored);
        }
    });
}

void Project::LoadUnpackedCommand::run()
{
    emit started();

    qDebug() << qPrintable(QString("Initializing \"%1\"...").arg(project->originalPath));
    project->journal(tr("Reading APK contents..."));

    project->filesystemModel.setRootPath(project->contentsPath);

    // Parse application manifest:

    project->manifest = new Manifest(
        project->contentsPath + "/AndroidManifest.xml",
        project->contentsPath + "/apktool.yml");
    project->manifestModel.initialize(project->manifest);

    // Parse resource directories:

    QMap<QString, QModelIndex> mapResourceTypes;
    QMap<QString, QModelIndex> mapResourceGroups;

    QDirIterator resourceDirectories(project->contentsPath + "/res/", QDir::Dirs | QDir::NoDotAndDotDot);
    while (resourceDirectories.hasNext()) {

        const QFileInfo resourceDirectory = QFileInfo(resourceDirectories.next());
        const QString resourceTypeTitle = resourceDirectory.fileName().split('-').first(); // E.g., "drawable", "values"...
        QModelIndex resourceTypeIndex = mapResourceTypes.value(resourceTypeTitle, QModelIndex());
        ResourceNode *resourceTypeNode = nullptr;
        if (resourceTypeIndex.isValid()) {
            resourceTypeNode = static_cast<ResourceNode *>(resourceTypeIndex.internalPointer());
        } else {
            resourceTypeNode = new ResourceNode(resourceTypeTitle, nullptr);
            resourceTypeIndex = project->resourcesModel.addNode(resourceTypeNode);
            mapResourceTypes[resourceTypeTitle] = resourceTypeIndex;
        }

        // Parse resource files:

        QDirIterator resourceFiles(resourceDirectory.filePath(), QDir::Files);
        while (resourceFiles.hasNext()) {

            const QFileInfo resourceFile(resourceFiles.next());
            const QString resourceFilename = resourceFile.fileName();

            QModelIndex resourceGroupIndex = mapResourceGroups.value(resourceFilename, QModelIndex());
            ResourceNode *resourceGroupNode = nullptr;
            if (resourceGroupIndex.isValid()) {
                resourceGroupNode = static_cast<ResourceNode *>(resourceGroupIndex.internalPointer());
            } else {
                resourceGroupNode = new ResourceNode(resourceFilename, nullptr);
                resourceGroupIndex = project->resourcesModel.addNode(resourceGroupNode, resourceTypeIndex);
                mapResourceGroups[resourceFilename] = resourceGroupIndex;
            }

            ResourceNode *fileNode = new ResourceNode(resourceFilename, new ResourceFile(resourceFile.filePath()));
            project->resourcesModel.addNode(fileNode, resourceGroupIndex);
        }
    }

    project->iconsProxy.sort();
    emit finished(true);
}
