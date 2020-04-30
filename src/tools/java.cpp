#include "tools/java.h"
#include "base/process.h"
#include "base/application.h"
#include <QRegularExpression>

void Java::Version::run()
{
    emit started();
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            QRegularExpression regex("version \"(.+)\"");
            resultVersion = regex.match(output).captured(1);
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(app->getJavaBinaryPath("java"), {"-version"});
}

const QString &Java::Version::version() const
{
    return resultVersion;
}
