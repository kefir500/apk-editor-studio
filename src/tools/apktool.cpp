#include "tools/apktool.h"
#include "base/jarprocess.h"
#include "base/application.h"
#include "base/settings.h"
#include "base/utils.h"
#include <QFile>
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

    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
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
    if (debuggable) {
        arguments << "--debug";
    }

    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Apktool::Build::output() const
{
    return resultOutput;
}

void Apktool::InstallFramework::run()
{
    emit started();

    QStringList arguments;
    arguments << "install-framework" << source;
    arguments << "--frame-path" << destination;

    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Apktool::InstallFramework::output() const
{
    return resultOutput;
}

void Apktool::Version::run()
{
    emit started();
    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        if (success) {
            resultVersion = output;
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), {"-version"});
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
    const QString path = Utils::toAbsolutePath(app->settings->getApktoolPath());
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Apktool::getDefaultPath()
{
    return Utils::getSharedPath("tools/apktool.jar");
}

QString Apktool::getOutputPath()
{
    const QString path = Utils::toAbsolutePath(app->settings->getOutputDirectory());
    return !path.isEmpty() ? path : getDefaultOutputPath();
}

QString Apktool::getDefaultOutputPath()
{
    return Utils::getTemporaryPath("apk");
}

QString Apktool::getFrameworksPath()
{
    const QString path = Utils::toAbsolutePath(app->settings->getFrameworksDirectory());
    return !path.isEmpty() ? path : getDefaultFrameworksPath();
}

QString Apktool::getDefaultFrameworksPath()
{
    return Utils::getLocalConfigPath("frameworks");
}
