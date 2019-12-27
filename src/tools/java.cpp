#include "tools/java.h"
#include "base/process.h"
#include "base/application.h"
#include <QRegularExpression>

void Java::Version::run()
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            QRegularExpression regex("version \"(.+)\"");
            const QString version = regex.match(output).captured(1);
            emit finished(version);
        } else {
            emit finished({});
        }
        process->deleteLater();
    });
    process->run(app->getJavaBinaryPath("java"), {"-version"});
}
