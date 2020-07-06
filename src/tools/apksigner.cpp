#include "tools/apksigner.h"
#include "base/process.h"
#include "base/application.h"
#include <QStringList>

void Apksigner::Sign::run()
{
    emit started();

    QStringList arguments;
    arguments << "sign";
    arguments << "--ks" << keystorePath;
    arguments << "--ks-key-alias" << keyAlias;
    arguments << "--ks-pass" << QString("pass:%1").arg(keystorePassword);
    arguments << "--key-pass" << QString("pass:%1").arg(keyPassword);
    arguments << target;

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->jar(getPath(), arguments);
}

const QString &Apksigner::Sign::output() const
{
    return resultOutput;
}

void Apksigner::Version::run()
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
    process->jar(getPath(), {"--version"});
}

const QString &Apksigner::Version::version() const
{
    return resultVersion;
}

QString Apksigner::getPath()
{
    const QString path = app->settings->getApksignerPath();
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Apksigner::getDefaultPath()
{
    return app->getSharedPath("tools/apksigner.jar");
}
