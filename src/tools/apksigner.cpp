#include "tools/apksigner.h"
#include "base/jarprocess.h"
#include "base/application.h"
#include "base/utils.h"
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

    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Apksigner::Sign::output() const
{
    return resultOutput;
}

void Apksigner::Version::run()
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
    process->run(getPath(), {"--version"});
}

const QString &Apksigner::Version::version() const
{
    return resultVersion;
}

QString Apksigner::getPath()
{
    const QString path = Utils::toAbsolutePath(app->settings->getApksignerPath());
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Apksigner::getDefaultPath()
{
    return Utils::getSharedPath("tools/apksigner.jar");
}
