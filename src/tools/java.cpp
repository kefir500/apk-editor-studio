#include "tools/java.h"
#include "base/application.h"
#include "base/process.h"
#include "base/settings.h"
#include <QDir>
#include <QRegularExpression>
#include <QSettings>

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
    const QString customPath = app->settings->getJavaPath();
    if (!customPath.isEmpty()) {
        return customPath;
    }

    const QString environmentPath = qEnvironmentVariable("JAVA_HOME");
    if (!environmentPath.isEmpty()) {
        return environmentPath;
    }

#ifdef Q_OS_WIN
    const QStringList registryKeyNames{
        "JDK", "Java Development Kit",
        "JRE", "Java Runtime Environment",
    };
    for (const auto &registryKeyName : registryKeyNames) {
        const QString registryKeyPath = QString("%1\\%2").arg("HKEY_LOCAL_MACHINE\\Software\\JavaSoft", registryKeyName);
        const QSettings registryJavaKey(registryKeyPath, QSettings::Registry64Format);
        const QString currentVersion = registryJavaKey.value("CurrentVersion").toString();
        if (!currentVersion.isEmpty()) {
            const QSettings registryJavaVersionKey(QString("%1\\%2").arg(registryKeyPath, currentVersion), QSettings::Registry64Format);
            const QString registryPath = registryJavaVersionKey.value("JavaHome").toString();
            if (!registryPath.isEmpty()) {
                return registryPath;
            }
        }
    }
#endif

    return QString();
}

QString Java::getBinaryPath(const QString &executable)
{
    const QString javaPath = getInstallationPath();
    if (!javaPath.isEmpty()) {
        return QDir(javaPath).filePath(QString("bin/%1").arg(executable));
    }

    return executable;
}
