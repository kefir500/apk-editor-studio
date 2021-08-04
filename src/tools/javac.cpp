#include "tools/javac.h"
#include "tools/java.h"
#include "base/process.h"
#include <QRegularExpression>

void Javac::Version::run()
{
    emit started();
    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        if (success) {
            QRegularExpression regex("javac (.+)");
            resultVersion = regex.match(output).captured(1);
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(Java::getBinaryPath("javac"), {"-version"});
}

const QString &Javac::Version::version() const
{
    return resultVersion;
}
