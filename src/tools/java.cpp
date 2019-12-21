#include "tools/java.h"
#include "base/process.h"
#include "base/application.h"
#include <QRegularExpression>

void Java::version()
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            QRegularExpression regex("version \"(.+)\"");
            const QString version = regex.match(output).captured(1);
            emit versionFetched(version);
        } else {
            emit versionFetched({});
        }
        process->deleteLater();
    });
    process->run(app->getJavaBinaryPath("java"), {"-version"});
}
