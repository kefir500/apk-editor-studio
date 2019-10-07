#include "tools/apktool.h"
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include "base/application.h"

void Apktool::decode(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources)
{
    if (source.isEmpty()) {
        emit error("Apktool: Source APK not specified.");
        return;
    }
    if (destination.isEmpty()) {
        emit error("Apktool: Destination path not specified.");
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
    Jar::startAsync(arguments);
}

void Apktool::build(const QString &source, const QString &destination, const QString &frameworks)
{
    if (source.isEmpty()) {
        emit error("Apktool: Source path not specified.");
        return;
    }
    if (destination.isEmpty()) {
        emit error("Apktool: Destination APK not specified.");
        return;
    }

    QStringList arguments;
    arguments << "build" << source;
    arguments << "--output" << destination;
    arguments << "--force";
    if (!frameworks.isEmpty()) {
        arguments << "--frame-path" << frameworks;
    }
    Jar::startAsync(arguments);
}

QString Apktool::version() const
{
    QStringList arguments;
    arguments << "-version";
    auto result = startSync(arguments);
    return result.success ? result.value : QString();
}

void Apktool::reset() const
{
    QtConcurrent::run([=]() {
        const QString currentVersion = version();
        const QString previousVersion = app->settings->getApktoolVersion();
        if (currentVersion.isNull() || currentVersion != previousVersion) {
            QFile::remove(getFrameworksPath() + "/1.apk");
            app->settings->setApktoolVersion(currentVersion);
        }
    });
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
