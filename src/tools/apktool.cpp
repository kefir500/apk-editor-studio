#include "tools/apktool.h"
#include "base/process.h"
#include "base/application.h"
#include <QStringList>

void Apktool::decode(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources, bool keepBroken)
{
    if (source.isEmpty()) {
        emit decodeFinished(false, "Apktool: Source APK not specified.");
        return;
    }
    if (destination.isEmpty()) {
        emit decodeFinished(false, "Apktool: Destination path not specified.");
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
    connect(process, &Process::finished, this, &Apktool::decodeFinished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->jar(getPath(), arguments);
}

void Apktool::build(const QString &source, const QString &destination, const QString &frameworks)
{
    if (source.isEmpty()) {
        emit buildFinished(false, "Apktool: Source path not specified.");
        return;
    }
    if (destination.isEmpty()) {
        emit buildFinished(false, "Apktool: Destination APK not specified.");
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
    connect(process, &Process::finished, this, &Apktool::buildFinished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->jar(getPath(), arguments);
}

void Apktool::version()
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        emit versionFetched(success ? output : QString());
        process->deleteLater();
    });
    process->jar(getPath(), {"-version"});
}

void Apktool::reset()
{
    auto context = new QObject(this);
    connect(this, &Apktool::versionFetched, context, [=](const QString &currentVersion) {
        const QString previousVersion = app->settings->getApktoolVersion();
        if (currentVersion.isNull() || currentVersion != previousVersion) {
            QFile::remove(getFrameworksPath() + "/1.apk");
            app->settings->setApktoolVersion(currentVersion);
        }
        emit resetFinished();
        delete context;
    });
    version();
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
