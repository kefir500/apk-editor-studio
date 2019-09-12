#include "tools/java.h"
#include "base/application.h"
#include <QRegularExpression>

Java::Java(QObject *parent) : Executable(parent)
{
    executable = findJavaCommand();
}

QString Java::version()
{
    QStringList arguments;
    arguments << "-version";
    auto result = startSync(arguments);
    if (!result.success) {
        return QString();
    }
    QRegularExpression regex("version \"(.+)\"");
    const QString version = regex.match(result.value).captured(1);
    return version;
}

QString Java::findJavaCommand()
{
    const QString userPath = app->settings->getJavaPath();
    if (!userPath.isEmpty()) {
        return userPath;
    }
    const QString envPath = qEnvironmentVariable("JAVA_HOME");
    if (!envPath.isEmpty()) {
        return QDir(envPath).filePath("bin/java");
    }
    return "java";
}
