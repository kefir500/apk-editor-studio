#include "apk/project.h"
#include "base/application.h"
#include "base/utils.h"
#include "tools/adb.h"
#include "tools/apktool.h"
#include "tools/apksigner.h"
#include "tools/zipalign.h"
#include <QUuid>
#include <QFutureWatcher>
#include <QDebug>

Project::Project(const QString &path) : resourcesModel(this)
{
    originalPath = QFileInfo(path).absoluteFilePath();
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

QString Project::getTitle() const
{
    return QFileInfo(originalPath).fileName();
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

void Project::setApplicationIcon(const QString &path, QWidget *parent)
{
    iconsProxy.replaceApplicationIcons(path, parent);
}

void Project::journal(const QString &brief, LogEntry::Type type)
{
    logModel.add(brief, QString(), type);
}

void Project::journal(const QString &brief, const QString &descriptive, LogEntry::Type type)
{
    logModel.add(brief, descriptive, type);
}

Command *Project::createUnpackCommand()
{
    QString source(getOriginalPath());
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
    contentsPath = target;

    auto apktoolDecode = new Apktool::Decode(source, target, frameworks, resources, sources, keepBroken);
    connect(apktoolDecode, &Command::finished, this, [=](bool success) {
        if (success) {
            filesystemModel.setRootPath(contentsPath);
        } else {
            journal(tr("Error unpacking APK."), apktoolDecode->output(), LogEntry::Error);
        }
    });

    auto command = new Commands(this);
    command->add(apktoolDecode, true);
    command->add(new LoadUnpackedCommand(this), true);
    connect(command, &Command::started, this, [=]() {
        qDebug() << qPrintable(QString("Unpacking\n  from: %1\n    to: %2\n").arg(source, target));
        journal(tr("Unpacking APK..."));
        state.setCurrentStatus(ProjectState::Status::Unpacking);
    });
    connect(command, &Command::finished, this, [=](bool success) {
        if (success) {
            connect(&resourcesModel, &ResourceItemsModel::dataChanged, [=]() {
                state.setModified(true);
            });
            connect(&filesystemModel, &QFileSystemModel::dataChanged, [=]() {
                state.setModified(true);
            });
            connect(&iconsProxy, &IconItemsModel::dataChanged, [=]() {
                state.setModified(true);
            });
            connect(&manifestModel, &ManifestModel::dataChanged, [=](const QModelIndex &, const QModelIndex &, const QVector<int> &roles) {
                if (!(roles.count() == 1 && roles.contains(Qt::UserRole))) {
                    state.setModified(true);
                }
            });
        }
        state.setUnpacked(success);
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
        state.setCurrentStatus(ProjectState::Status::Packing);
    });

    connect(apktoolBuild, &Command::finished, this, [=](bool success) {
        if (success) {
            originalPath = target;
            state.setModified(false);
        } else {
            journal(tr("Error packing APK."), apktoolBuild->output(), LogEntry::Error);
        }
    });

    return apktoolBuild;
}

Command *Project::createZipalignCommand(const QString &apk)
{
    auto zipalign = new Zipalign::Align(apk.isEmpty() ? getOriginalPath() : apk);

    connect(zipalign, &Command::started, this, [=]() {
        journal(tr("Optimizing APK..."));
        state.setCurrentStatus(ProjectState::Status::Optimizing);
    });

    connect(zipalign, &Command::finished, this, [=](bool success) {
        if (!success) {
            journal(tr("Error optimizing APK."), zipalign->output(), LogEntry::Error);
        }
    });

    return zipalign;
}

Command *Project::createSignCommand(const Keystore *keystore, const QString &apk)
{
    auto apksigner = new Apksigner::Sign(apk.isEmpty() ? getOriginalPath() : apk, keystore);

    connect(apksigner, &Command::started, this, [=]() {
        journal(tr("Signing APK..."));
        state.setCurrentStatus(ProjectState::Status::Signing);
    });

    connect(apksigner, &Command::finished, [=](bool success) {
        if (!success) {
            journal(tr("Error signing APK."), apksigner->output(), LogEntry::Error);
        }
    });

    return apksigner;
}

Command *Project::createInstallCommand(const QString &serial, const QString &apk)
{
    auto install = new Adb::Install(apk.isEmpty() ? getOriginalPath() : apk, serial);

    connect(install, &Command::started, this, [=]() {
        journal(tr("Installing APK..."));
        state.setCurrentStatus(ProjectState::Status::Installing);
    });

    connect(install, &Command::finished, this, [=](bool success) {
        if (!success) {
            journal(tr("Error installing APK."), install->output(), LogEntry::Error);
        }
    });

    return install;
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
            project->journal(Project::tr("Done."), LogEntry::Success);
            project->state.setCurrentStatus(ProjectState::Status::Normal);
        } else {
            project->state.setCurrentStatus(ProjectState::Status::Errored);
        }
        app->actions.addToRecent(project);
    });
}

void Project::LoadUnpackedCommand::run()
{
    emit started();

    project->journal(Project::tr("Reading APK contents..."));
    project->manifest = new Manifest(
        project->contentsPath + "/AndroidManifest.xml",
        project->contentsPath + "/apktool.yml");
    project->manifestModel.initialize(project->manifest);

    auto initResourcesFuture = project->resourcesModel.initialize(project->contentsPath + "/res/");
    auto initResourcesFutureWatcher = new QFutureWatcher<void>(this);
    connect(initResourcesFutureWatcher, &QFutureWatcher<void>::finished, [=]() {
        emit finished(true);
    });
    initResourcesFutureWatcher->setFuture(initResourcesFuture);
}
