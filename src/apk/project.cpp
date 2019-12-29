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
    connect(&state, &ProjectState::changed, [this]() {
        logModel.setLoadingState(!state.isIdle());
        emit changed();
    });
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
    logModel.clear();

    auto taskOpen = createUnpackTask(getOriginalPath());

    connect(taskOpen, &Tasks::Task::success, this, [=]() {
        journal(tr("Done."), LogEntry::Success);
        state.setCurrentState(ProjectState::StateNormal);
    }, Qt::QueuedConnection);

    connect(taskOpen, &Tasks::Task::error, this, [=]() {
        state.setCurrentState(ProjectState::StateErrored);
    }, Qt::QueuedConnection);

    taskOpen->run();
}

void Project::save(QString path)
{
    logModel.clear();

    path = QDir::toNativeSeparators(path);
    QFileInfo fileInfo(path);
    const QString directory = fileInfo.absolutePath();
    app->settings->setLastDirectory(directory);

    auto taskSave = createSaveTask(path);

    connect(taskSave, &Tasks::Batch::success, this, [=]() {
        journal(tr("Done."), LogEntry::Success);
        state.setCurrentState(ProjectState::StateNormal);
    }, Qt::QueuedConnection);

    connect(taskSave, &Tasks::Batch::error, this, [=]() {
        state.setCurrentState(ProjectState::StateErrored);
    }, Qt::QueuedConnection);

    taskSave->run();
}

void Project::saveAndInstall(QString path, const QString &serial)
{
    logModel.clear();

    path = QDir::toNativeSeparators(path);
    QFileInfo fileInfo(path);
    const QString directory = fileInfo.absolutePath();
    app->settings->setLastDirectory(directory);

    auto tasks = new Tasks::Batch;

    tasks->add(createSaveTask(path), true);
    tasks->add(createInstallTask(serial), true);

    connect(tasks, &Tasks::Batch::success, this, [=]() {
        journal(tr("Done."), LogEntry::Success);
        state.setCurrentState(ProjectState::StateNormal);
    }, Qt::QueuedConnection);

    connect(tasks, &Tasks::Batch::error, this, [=]() {
        state.setCurrentState(ProjectState::StateErrored);
    }, Qt::QueuedConnection);

    tasks->run();
}

void Project::install(const QString &serial)
{
    logModel.clear();

    auto taskInstall = createInstallTask(serial);

    connect(taskInstall, &Tasks::Install::success, this, [=]() {
        journal(tr("Done."), LogEntry::Success);
        state.setCurrentState(ProjectState::StateNormal);
    }, Qt::QueuedConnection);

    connect(taskInstall, &Tasks::Install::error, this, [=]() {
        state.setCurrentState(ProjectState::StateErrored);
    }, Qt::QueuedConnection);

    taskInstall->run();
}

Manifest *Project::initialize()
{
    qDebug() << qPrintable(QString("Initializing \"%1\"...").arg(getOriginalPath()));

    filesystemModel.setRootPath(contentsPath);

    // Parse application manifest:

    manifest = new Manifest(contentsPath + "/AndroidManifest.xml", contentsPath + "/apktool.yml");
    manifestModel.initialize(manifest);

    // Parse resource directories:

    QMap<QString, QModelIndex> mapResourceTypes;
    QMap<QString, QModelIndex> mapResourceGroups;

    QDirIterator resourceDirectories(contentsPath + "/res/", QDir::Dirs | QDir::NoDotAndDotDot);
    while (resourceDirectories.hasNext()) {

        const QFileInfo resourceDirectory = QFileInfo(resourceDirectories.next());
        const QString resourceTypeTitle = resourceDirectory.fileName().split('-').first(); // E.g., "drawable", "values"...
        QModelIndex resourceTypeIndex = mapResourceTypes.value(resourceTypeTitle, QModelIndex());
        ResourceNode *resourceTypeNode = nullptr;
        if (resourceTypeIndex.isValid()) {
            resourceTypeNode = static_cast<ResourceNode *>(resourceTypeIndex.internalPointer());
        } else {
            resourceTypeNode = new ResourceNode(resourceTypeTitle, nullptr);
            resourceTypeIndex = resourcesModel.addNode(resourceTypeNode);
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
                resourceGroupIndex = resourcesModel.addNode(resourceGroupNode, resourceTypeIndex);
                mapResourceGroups[resourceFilename] = resourceGroupIndex;
            }

            ResourceNode *fileNode = new ResourceNode(resourceFilename, new ResourceFile(resourceFile.filePath()));
            resourcesModel.addNode(fileNode, resourceGroupIndex);
        }
    }

    iconsProxy.sort();

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

    qDebug();
    return manifest;
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

Tasks::Task *Project::createUnpackTask(const QString &source)
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

    auto taskUnpack = new Tasks::Unpack(source, target, frameworks, resources, sources, keepBroken);

    connect(taskUnpack, &Tasks::Pack::started, this, [=]() {
        journal(tr("Unpacking APK..."));
        qDebug() << qPrintable(QString("Unpacking\n  from: %1\n    to: %2\n").arg(source, target));
        state.setCurrentState(ProjectState::StateUnpacking);
    }, Qt::QueuedConnection);

    connect(taskUnpack, &Tasks::Unpack::success, this, [=]() {
        journal(tr("Reading APK contents..."));
        initialize();
        state.setUnpacked(true);
        emit unpacked(true);
    }, Qt::QueuedConnection);

    connect(taskUnpack, &Tasks::Unpack::error, this, [=](const QString &message) {
        journal(tr("Error unpacking APK."), message, LogEntry::Error);
        emit unpacked(false);
    }, Qt::QueuedConnection);

    return taskUnpack;
}

Tasks::Task *Project::createSaveTask(const QString &target) // Combines Pack, Zipalign and Sign tasks
{
    auto taskSave = new Tasks::Batch;

    // Pack APK:

    taskSave->add(createPackTask(target), true);

    // Optimize APK:

    if (app->settings->getOptimizeApk()) {
        taskSave->add(createZipalignTask(target));
    }

    // Sign APK:

    if (app->settings->getSignApk()) {
        const Keystore *keystore = getKeystore();
        if (keystore) {
            taskSave->add(createSignTask(target, keystore));
        }
    }

    // Done:

    connect(taskSave, &Tasks::Batch::finished, this, [=]() {
        QFileInfo fileInfo(target);
        title = fileInfo.fileName();
        originalPath = target;
    });

    return taskSave;
}

Tasks::Task *Project::createPackTask(const QString &target)
{
    const QString source = getContentsPath();
    const QString frameworks = Apktool::getFrameworksPath();

    auto taskPack = new Tasks::Pack(source, target, frameworks);

    connect(taskPack, &Tasks::Pack::started, this, [=]() {
        journal(tr("Packing APK..."));
        qDebug() << qPrintable(QString("Packing\n  from: %1\n    to: %2\n").arg(source, target));
        state.setCurrentState(ProjectState::StatePacking);
    }, Qt::QueuedConnection);

    connect(taskPack, &Tasks::Pack::success, this, [=]() {
        state.setModified(false);
        emit packed(true);
    });

    connect(taskPack, &Tasks::Pack::error, this, [=](const QString &message) {
        journal(tr("Error packing APK."), message, LogEntry::Error);
        emit packed(false);
    }, Qt::QueuedConnection);

    return taskPack;
}

Tasks::Task *Project::createZipalignTask(const QString &target)
{
    auto taskZipalign = new Tasks::Align(target);

    connect(taskZipalign, &Tasks::Align::started, this, [=]() {
        state.setCurrentState(ProjectState::StateOptimizing);
        journal(tr("Optimizing APK..."));
    }, Qt::QueuedConnection);

    connect(taskZipalign, &Tasks::Align::error, this, [=](const QString &message) {
        journal(tr("Error optimizing APK."), message, LogEntry::Error);
    }, Qt::QueuedConnection);

    return taskZipalign;
}

Tasks::Task *Project::createSignTask(const QString &target, const Keystore *keystore)
{
    auto taskSign = new Tasks::Sign(target, keystore);

    connect(taskSign, &Tasks::Sign::finished, [=]() {
        delete keystore;
    });

    connect(taskSign, &Tasks::Sign::started, this, [=]() {
        state.setCurrentState(ProjectState::StateSigning);
        journal(tr("Signing APK..."));
    }, Qt::QueuedConnection);

    connect(taskSign, &Tasks::Sign::error, this, [=](const QString &message) {
        journal(tr("Error signing APK."), message, LogEntry::Error);
    }, Qt::QueuedConnection);

    return taskSign;
}

Tasks::Task *Project::createInstallTask(const QString &serial)
{
    auto taskInstall = new Tasks::Install(originalPath, serial);

    connect(taskInstall, &Tasks::Install::started, this, [=]() {
        journal(tr("Installing APK..."));
        state.setCurrentState(ProjectState::StateInstalling);
    }, Qt::QueuedConnection);

    connect(taskInstall, &Tasks::Install::success, this, [=]() {
        emit installed(true);
    }, Qt::QueuedConnection);

    connect(taskInstall, &Tasks::Install::error, this, [=](const QString &message) {
        journal(tr("Error installing APK."), message, LogEntry::Error);
        emit installed(false);
    }, Qt::QueuedConnection);

    return taskInstall;
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
