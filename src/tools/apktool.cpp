#include "tools/apktool.h"
#include "base/process.h"
#include "base/application.h"
#include <QStringList>

void Apktool::Decode::run()
{
    if (source.isEmpty()) {
        emit finished(false, "Apktool: Source APK not specified.");
        return;
    }
    if (destination.isEmpty()) {
        emit finished(false, "Apktool: Destination path not specified.");
        return;
    }

    QStringList arguments;
    arguments << "decode" << source;
    arguments << "--output" << destination;
    arguments << "--force";
    if (!frameworks.isEmpty()) {
        arguments << "--frame-path" << frameworks;
    }
    if (!resources) {
        arguments << "--no-res";
    }
    if (!sources) {
        arguments << "--no-src";
    }
    if (keepBroken) {
        arguments << "--keep-broken-res";
    }

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Decode::finished);
    connect(process, &Process::finished, process, &Process::deleteLater);
    process->jar(getPath(), arguments);
}

void Apktool::Build::run()
{
    if (source.isEmpty()) {
        emit finished(false, "Apktool: Source path not specified.");
        return;
    }
    if (destination.isEmpty()) {
        emit finished(false, "Apktool: Destination APK not specified.");
        return;
    }

    QStringList arguments;
    arguments << "build" << source;
    arguments << "--output" << destination;
    arguments << "--force";
    if (!frameworks.isEmpty()) {
        arguments << "--frame-path" << frameworks;
    }

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Build::finished);
    connect(process, &Process::finished, process, &Process::deleteLater);
    process->jar(getPath(), arguments);
}

void Apktool::Version::run()
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        emit finished(success ? output : QString());
        process->deleteLater();
    });
    process->jar(getPath(), {"-version"});
}

void Apktool::reset()
{
    auto versionCommand = new Version;
    // TODO 111 Is app->connect deleted after Version destruction?
    app->connect(versionCommand, &Version::finished, [=](const QString &currentVersion) {
        const QString previousVersion = app->settings->getApktoolVersion();
        if (currentVersion.isNull() || currentVersion != previousVersion) {
            QFile::remove(getFrameworksPath() + "/1.apk");
            app->settings->setApktoolVersion(currentVersion);
        }
    });
    versionCommand->run();
}

QString Apktool::getPath()
{
    const QString path = app->settings->getApktoolPath();
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Apktool::getDefaultPath()
{
    return app->getSharedPath("tools/apktool.jar");
}

QString Apktool::getOutputPath()
{
    const QString path = app->settings->getOutputDirectory();
    return !path.isEmpty() ? path : getDefaultOutputPath();
}

QString Apktool::getDefaultOutputPath()
{
    return app->getTemporaryPath("apk");
}

QString Apktool::getFrameworksPath()
{
    const QString path = app->settings->getFrameworksDirectory();
    return !path.isEmpty() ? path : getDefaultFrameworksPath();
}

QString Apktool::getDefaultFrameworksPath()
{
    return app->getLocalConfigPath("frameworks");
}
