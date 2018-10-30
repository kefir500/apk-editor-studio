#include "apk/project.h"
#include "base/application.h"
#include "base/debug.h"
#include "tools/apktool.h"
#include "tools/apksigner.h"
#include "tools/zipalign.h"
#include "tools/adb.h"
#include "windows/dialogs.h"
#include "windows/keymanager.h"
#include <QThreadPool>
#include <QUuid>
#include <QDirIterator>
#include <QInputDialog>

Project::Project(const QString &path)
{
    QFileInfo fileInfo(path);
    title = fileInfo.fileName();
    originalPath = path;
    manifest = nullptr;
    isUnpacked = false;
    setState(ProjectEmpty);
    setErrored(false);
    setModified(false);
    iconsProxy.setSourceModel(&resourcesModel);
}

Project::~Project()
{
    if (manifest) {
        delete manifest;
    }
    if (!contentsPath.isEmpty()) {
        qDebug() << qPrintable(QString("Removing \"%1\"...").arg(contentsPath));
        // Additional check to prevent accidental recursive deletion of the wrong directory:
        const bool recursive = QFile::exists(QString("%1/%2").arg(contentsPath, "AndroidManifest.xml"));
        app->rmdir(contentsPath, recursive);
    }
}

void Project::unpack()
{
    logModel.clear();

    auto taskOpen = createUnpackTask(originalPath);

    connect(taskOpen, &Tasks::Task::started, this, [=]() {
        setErrored(false);
    }, Qt::QueuedConnection);

    connect(taskOpen, &Tasks::Task::success, this, [=]() {
        setState(ProjectReady);
        journal(tr("Done."), LogEntry::Success);
    }, Qt::QueuedConnection);

    connect(taskOpen, &Tasks::Task::error, this, [=]() {
        setState(ProjectEmpty);
        setErrored(true);
    }, Qt::QueuedConnection);

    taskOpen->run();
}

void Project::pack(const QString &path)
{
    logModel.clear();

    QFileInfo fileInfo(path);
    const QString directory = fileInfo.absolutePath();
    app->settings->setLastDirectory(directory);

    auto taskSave = createSaveTask(path);

    connect(taskSave, &Tasks::Batch::started, this, [=]() {
        setErrored(false);
    }, Qt::QueuedConnection);

    connect(taskSave, &Tasks::Batch::success, this, [=]() {
        setState(ProjectReady);
        setModified(false);
        journal(tr("Done."), LogEntry::Success);
    }, Qt::QueuedConnection);

    connect(taskSave, &Tasks::Batch::error, this, [=]() {
        setState(ProjectReady);
        setErrored(true);
    }, Qt::QueuedConnection);

    taskSave->run();
}

void Project::saveAndInstall(const QString &path, const QString &serial)
{
    logModel.clear();

    QFileInfo fileInfo(path);
    const QString directory = fileInfo.absolutePath();
    app->settings->setLastDirectory(directory);

    auto tasks = new Tasks::Batch;

    tasks->add(createSaveTask(path), true);
    tasks->add(createInstallTask(serial), true);

    connect(tasks, &Tasks::Batch::started, this, [=]() {
        setErrored(false);
    }, Qt::QueuedConnection);

    connect(tasks, &Tasks::Batch::success, this, [=]() {
        setState(ProjectReady);
        journal(tr("Done."), LogEntry::Success);
    }, Qt::QueuedConnection);

    connect(tasks, &Tasks::Batch::error, this, [=]() {
        setState(ProjectReady);
        setErrored(true);
    }, Qt::QueuedConnection);

    tasks->run();
}

void Project::install(const QString &serial)
{
    logModel.clear();

    auto taskInstall = createInstallTask(serial);

    connect(taskInstall, &Tasks::Install::started, this, [=]() {
        setErrored(false);
    }, Qt::QueuedConnection);

    connect(taskInstall, &Tasks::Install::success, this, [=]() {
        setState(ProjectReady);
        journal(tr("Done."), LogEntry::Success);
    }, Qt::QueuedConnection);

    connect(taskInstall, &Tasks::Install::error, this, [=]() {
        setState(isUnpacked ? ProjectReady : ProjectEmpty);
        setErrored(true);
    }, Qt::QueuedConnection);

    taskInstall->run();
}

Manifest *Project::initialize()
{
    filesystemModel.setRootPath(contentsPath);

    // Parse application manifest:

    manifest = new Manifest(contentsPath + "/AndroidManifest.xml", contentsPath + "/apktool.yml");
    manifestModel.initialize(manifest);

    // Parse application icon attribute (android:icon):

    QString iconAttribute = manifest->getApplicationIcon();
    QString iconCategory = iconAttribute.split('/').value(0).mid(1);
    QString iconFilename = iconAttribute.split('/').value(1);

    // Parse resource directories:

    QMap<QString, QModelIndex> mapCategories;
    QMap<QString, QModelIndex> mapFilegroups;

    QDirIterator resourceDirectories(contentsPath + "/res/", QDir::Dirs | QDir::NoDotAndDotDot);
    while (resourceDirectories.hasNext()) {

        const QString resourceDirectory = QFileInfo(resourceDirectories.next()).fileName();
        const QString categoryTitle = resourceDirectory.split('-').first(); // E.g., "drawable", "values"...
        QModelIndex categoryIndex = mapCategories.value(categoryTitle, QModelIndex());
        ResourceNode *categoryNode = nullptr;
        if (categoryIndex.isValid()) {
            categoryNode = static_cast<ResourceNode *>(categoryIndex.internalPointer());
        } else {
            categoryNode = new ResourceNode(categoryTitle, nullptr);
            categoryIndex = resourcesModel.addNode(categoryNode);
            mapCategories[categoryTitle] = categoryIndex;
        }

        // Parse resource files:

        QDirIterator resourceFiles(contentsPath + "/res/" + resourceDirectory, QDir::Files);
        while (resourceFiles.hasNext()) {

            const QString resourceFile = QFileInfo(resourceFiles.next()).fileName();
            QModelIndex filegroupIndex = mapFilegroups.value(resourceFile, QModelIndex());
            ResourceNode *filegroupNode = nullptr;
            if (filegroupIndex.isValid()) {
                filegroupNode = static_cast<ResourceNode *>(filegroupIndex.internalPointer());
            } else {
                filegroupNode = new ResourceNode(resourceFile, nullptr);
                filegroupIndex = resourcesModel.addNode(filegroupNode, categoryIndex);
                mapFilegroups[resourceFile] = filegroupIndex;
            }

            ResourceNode *fileNode = new ResourceNode(resourceFile, new ResourceFile(QString("%1/res/%2/%3").arg(contentsPath, resourceDirectory, resourceFile)));
            QModelIndex fileIndex = resourcesModel.addNode(fileNode, filegroupIndex);

            // Parse application icons:

            const QStringList allowedIconFiles = {(iconFilename + ".png"), (iconFilename + ".jpg"), (iconFilename + ".gif"), (iconFilename + ".xml")}; // Read more: https://developer.android.com/guide/topics/resources/drawable-resource.html
            if (categoryTitle == iconCategory && allowedIconFiles.contains(resourceFile)) {
                qDebug() << "Parsed application icon:" << fileNode->getFile()->getFilePath();
                iconsProxy.addIcon(fileIndex);
            }
        }
    }

    connect(&resourcesModel, &ResourcesModel::dataChanged, [=] () {
        setModified(true);
    });
    connect(&filesystemModel, &QFileSystemModel::dataChanged, [=] () {
        setModified(true);
    });
    connect(&iconsProxy, &IconsProxy::dataChanged, [=] () {
        setModified(true);
    });
    connect(&manifestModel, &ManifestModel::dataChanged, [=] (const QModelIndex &, const QModelIndex &, const QVector<int> &roles) {
        if (roles.contains(Qt::DisplayRole) || roles.contains(Qt::EditRole)) {
            setModified(true);
        }
    });

    qDebug();
    return manifest;
}

const QString &Project::getTitle() const
{
    return title;
}

const QString &Project::getOriginalPath() const
{
    return originalPath;
}

const QString &Project::getContentsPath() const
{
    return contentsPath;
}

const Manifest *Project::getManifest() const
{
    return manifest;
}

QIcon Project::getThumbnail() const
{
    QIcon thumbnail;
    const int rows = iconsProxy.rowCount();
    for (int i = 0; i < rows; ++i) {
        thumbnail.addPixmap(iconsProxy.index(i, 0).data(Qt::DecorationRole).value<QPixmap>());
    }
    return thumbnail;
}

Project::State Project::getState() const
{
    return state;
}

bool Project::getErroredState() const
{
    return isErrored;
}

bool Project::getModifiedState() const
{
    return isModified;
}

void Project::setModified(bool modified)
{
    isModified = modified;
    emit dataChanged();
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
        target = QString("%1/%2").arg(app->settings->getOutputDirectory(), uuid);
    } while (target.isEmpty() || QDir(target).exists());
    const QString frameworks = app->settings->getFrameworksDirectory();
    const bool resources = true;
    const bool sources = app->settings->getDecompileSources();

    QDir().mkpath(target);
    QDir().mkpath(frameworks);

    // Be careful with the "contentsPath" variable: this directory is recursively removed in the destructor.
    this->contentsPath = target;

    auto taskUnpack = new Tasks::Unpack(source, target, frameworks, resources, sources);

    connect(taskUnpack, &Tasks::Pack::started, this, [=]() {
        journal(tr("Unpacking APK..."));
        qDebug() << qPrintable(QString("Unpacking\n  from: %1\n  to:   %2\n").arg(source, target));
        setState(ProjectUnpacking);
        setErrored(false);
    }, Qt::QueuedConnection);

    connect(taskUnpack, &Tasks::Unpack::success, this, [=]() {
        // Read manifest:
        journal(tr("Reading Android manifest..."));
        initialize();
        emit dataChanged();
        // Done:
        isUnpacked = true;
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

    connect(taskSave, &Tasks::Batch::success, this, [=]() {
        emit packed(true);
    }, Qt::QueuedConnection);

    connect(taskSave, &Tasks::Batch::error, this, [=]() {
        setModified(false);
        emit packed(false);
    }, Qt::QueuedConnection);

    return taskSave;
}

Tasks::Task *Project::createPackTask(const QString &target)
{
    const QString source = getContentsPath();
    const QString frameworks = app->settings->getFrameworksDirectory();
    const bool resources = true;
    const bool sources = !app->settings->getDecompileSources();

    auto taskPack = new Tasks::Pack(source, target, frameworks, resources, sources);

    connect(taskPack, &Tasks::Pack::started, this, [=]() {
        journal(tr("Packing APK..."));
        qDebug() << qPrintable(QString("Packing\n  from: %1\n  to:   %2\n").arg(source, target));
        setState(ProjectPacking);
    }, Qt::QueuedConnection);

    connect(taskPack, &Tasks::Pack::error, this, [=](const QString &message) {
        journal(tr("Error packing APK."), message, LogEntry::Error);
    }, Qt::QueuedConnection);

    return taskPack;
}

Tasks::Task *Project::createZipalignTask(const QString &target)
{
    auto taskZipalign = new Tasks::Align(target);

    connect(taskZipalign, &Tasks::Align::started, this, [=]() {
        setState(ProjectOptimizing);
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
        setState(ProjectSigning);
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
        setState(ProjectInstalling);
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
            keystore->keyAlias = KeyManager::selectKey(keystore->keystorePath, keystore->keystorePassword);
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

void Project::setState(State state)
{
    this->state = state;

    switch (state) {
    case ProjectEmpty:
    case ProjectReady:
        logModel.setLoadingState(false);
        break;
    default:
        logModel.setLoadingState(true);
        break;
    }

    emit dataChanged();
}

void Project::setErrored(bool errored)
{
    isErrored = errored;
    emit dataChanged();
}
