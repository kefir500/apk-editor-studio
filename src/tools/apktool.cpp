#include "tools/apktool.h"
#include "base/process.h"
#include "base/application.h"
#include <QStringList>

void Apktool::Decode::run()
{
    emit started();

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
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->jar(getPath(), arguments);
}

const QString &Apktool::Decode::output() const
{
    return resultOutput;
}

void Apktool::Build::run()
{
    emit started();

    QStringList arguments;
    arguments << "build" << source;
    arguments << "--output" << destination;
    arguments << "--force";
    if (!frameworks.isEmpty()) {
        arguments << "--frame-path" << frameworks;
    }
    if (aapt2) {
        arguments << "--use-aapt2";
    }

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->jar(getPath(), arguments);
}

const QString &Apktool::Build::output() const
{
    return resultOutput;
}

void Apktool::Version::run()
{
    emit started();
    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        if (success) {
            resultVersion = output;
        }
        emit finished(success);
        process->deleteLater();
    });
    process->jar(getPath(), {"-version"});
}

const QString &Apktool::Version::version() const
{
    return resultVersion;
}

void Apktool::reset()
{
    auto versionCommand = new Version;
    app->connect(versionCommand, &Version::finished, [=]() {
        const QString currentVersion = versionCommand->version();
        const QString previousVersion = app->settings->getApktoolVersion();
        if (currentVersion.isNull() || currentVersion != previousVersion) {
            QFile::remove(getFrameworksPath() + "/1.apk");
            app->settings->setApktoolVersion(currentVersion);
        }
        versionCommand->deleteLater();
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
