#include "tools/apktool.h"
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include "base/application.h"

void Apktool::build(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources)
{
    run("build", source, destination, frameworks, resources, sources);
}

void Apktool::decode(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources)
{
    run("decode", source, destination, frameworks, resources, sources);
}

QString Apktool::version() const
{
    QStringList arguments;
    arguments << "-version";
    auto result = startSync(arguments);
    return result.success ? result.value : QString();
}

void Apktool::run(const QString &action, const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources)
{
    if (source.isEmpty()) {
        emit error("Apktool: Source path not specified.");
        return;
    }
    if (destination.isEmpty()) {
        emit error("Apktool: Destination path not specified.");
        return;
    }

    QStringList arguments;
    arguments << action << source;
    arguments << "--output" << destination;
    arguments << "--force";
    if (!frameworks.isEmpty()) { arguments << "--frame-path" << frameworks; }
    if (!resources) { arguments << "--no-res"; }
    if (!sources) { arguments << "--no-src"; }

    Jar::startAsync(arguments);
}

void Apktool::reset() const
{
    QtConcurrent::run([=]() {
        const QString currentVersion = version();
        const QString previousVersion = app->settings->getApktoolVersion();
        if (currentVersion.isNull() || currentVersion != previousVersion) {
            qDebug() << qPrintable(QString("New Apktool version (from \"%1\" to \"%2\"). Removing 1.apk...").arg(previousVersion, currentVersion));
            QFile::remove(app->settings->getFrameworksDirectory() + "/1.apk");
            app->settings->setApktoolVersion(currentVersion);
        }
    });
}
