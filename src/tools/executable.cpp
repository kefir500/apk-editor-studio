#include "tools/executable.h"
#include <QDebug>

Executable::Executable(QObject *parent) : QObject(parent)
{
#ifndef Q_OS_OSX
    const int processKillCode = 0xF291; // Windows kill code (Qt magic number)
#else
    const int processKillCode = 9;      // SIGKILL code
#endif

    connect(&process, static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error), [=](QProcess::ProcessError processError) {
        switch (processError) {
        case QProcess::FailedToStart:
            emit error(QString("%1 (\"%2\")").arg(process.errorString(), this->executable));
            emit finished();
            break;
        default:
            qWarning() << QString("Unhandled process error (\"%1\"):\n%2").arg(this->executable, process.errorString());
            break;
        }
    });

    connect(&process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), [=]() {
        const QString output =
            process.readAllStandardOutput().replace("\r\n", "\n").trimmed() +
            process.readAllStandardError().replace("\r\n", "\n").trimmed();
        const int exitCode = process.exitCode();
        if (exitCode == 0 && process.exitStatus() == QProcess::NormalExit) {
            emit success(output);
        } else if (exitCode == processKillCode && process.exitStatus() == QProcess::CrashExit) {
            // Process killed
        } else {
            emit error(output);
        }
        emit finished();
    });
}

Executable::Executable(const QString &executable, QObject *parent) : Executable(parent)
{
    this->executable = executable;
}

void Executable::startAsync(const QStringList &arguments)
{
    process.start(executable, arguments);
}

Result<QString> Executable::startSync(const QStringList &arguments) const
{
    QProcess process;
    process.start(executable, arguments);

    if (!process.waitForStarted(10000)) {
        const QString error = QString("Error starting \"%1\"...\n%2").arg(executable, process.errorString());
        return Result<QString>(false, error);
    }

    if (!process.waitForFinished(-1)) {
        const QString error = process.readAllStandardError().replace("\r\n", "\n").trimmed();
        return Result<QString>(false, error);
    }

    if (process.exitCode() != 0) {
        const QString error = process.readAllStandardError().replace("\r\n", "\n").trimmed() +
                              process.readAllStandardOutput().replace("\r\n", "\n").trimmed();
        return Result<QString>(false, error);
    }

    const QString output =
        process.readAllStandardOutput().replace("\r\n", "\n").trimmed() +
        process.readAllStandardError().replace("\r\n", "\n").trimmed();
    return Result<QString>(true, output);
}
