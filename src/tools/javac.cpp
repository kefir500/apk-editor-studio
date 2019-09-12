#include "tools/javac.h"
#include "base/application.h"
#include <QRegularExpression>

Javac::Javac(QObject *parent) : Executable(parent)
{
    executable = findJdkBinPath() + "javac";
}

QString Javac::version() const
{
    QStringList arguments;
    arguments << "-version";
    auto result = startSync(arguments);
    if (!result.success) {
        return QString();
    }
    QRegularExpression regex("javac (.+)");
    const QString version = regex.match(result.value).captured(1);
    return version;
}

QString Javac::findJdkBinPath()
{
    QString path = app->settings->getJdkPath();
    if (path.isEmpty()) {
        path = qgetenv("JAVA_HOME");
    }
    if (path.isEmpty()) {
        return QString();
    }
    return QDir(path).filePath("bin/");
}
