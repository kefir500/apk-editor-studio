#include "tools/java.h"
#include "base/application.h"
#include "base/process.h"
#include "base/settings.h"
#include <QDir>
#include <QRegularExpression>

void Java::Version::run()
{
    emit started();
    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        if (success) {
            QRegularExpression regex("version \"(.+)\"");
            resultVersion = regex.match(output).captured(1);
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(Java::getBinaryPath("java"), {"-version"});
}

const QString &Java::Version::version() const
{
    return resultVersion;
}

QString Java::getInstallationPath()
{
    const QString userPath = app->settings->getJavaPath();
    if (!userPath.isEmpty()) {
        return userPath;
    }
    const QString envPath = qEnvironmentVariable("JAVA_HOME");
    if (!envPath.isEmpty()) {
        return envPath;
    }
    return QString();
}

QString Java::getBinaryPath(const QString &executable)
{
    const QString sdkPath = getInstallationPath();
    if (!sdkPath.isEmpty()) {
        return QDir(sdkPath).filePath(QString("bin/%1").arg(executable));
    }
    return executable;
}
