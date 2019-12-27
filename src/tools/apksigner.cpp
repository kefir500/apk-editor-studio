#include "tools/apksigner.h"
#include "base/process.h"
#include "base/application.h"
#include <QStringList>

void Apksigner::Sign::run()
{
    if (target.isEmpty()) {
        emit finished(false, "Apksigner: Target path not specified.");
        return;
    }

    QStringList arguments;
    arguments << "sign";
    arguments << "--ks" << keystorePath;
    arguments << "--ks-key-alias" << keyAlias;
    arguments << "--ks-pass" << QString("pass:%1").arg(keystorePassword);
    arguments << "--key-pass" << QString("pass:%1").arg(keyPassword);
    arguments << target;

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Apksigner::Sign::finished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->jar(getPath(), arguments);
}

void Apksigner::Version::run()
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        emit finished(success ? output : QString());
        process->deleteLater();
    });
    process->jar(getPath(), {"--version"});
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
