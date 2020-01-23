#include "base/process.h"
#include "base/application.h"
#include <QProcess>
#include <QDebug>

Process::Process(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_WIN
    const int processKillCode = 0xF291; // Windows kill code (Qt magic number)
#else
    const int processKillCode = 9;      // SIGKILL code
#endif

    process.setProcessChannelMode(QProcess::MergedChannels);

    connect(&process, &QProcess::started, this, &Process::started);

    connect(&process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus)
    {
        const QString output = process.readAll().replace("\r\n", "\n").trimmed();
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            emit finished(true, output);
        } else if (exitStatus == QProcess::CrashExit && exitCode == processKillCode) {
            // Process killed
            emit finished(true, {});
        } else {
            emit finished(false, output);
        }
        process.deleteLater();
    });

    connect(&process, static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
        [=](QProcess::ProcessError processError)
    {
        Q_UNUSED(processError)
        const auto error = QStringLiteral("%1: %2").arg(process.program(), process.errorString());
        emit finished(false, error);
        process.deleteLater();
    });

}

void Process::run(const QString &program, const QStringList &arguments)
{
    process.start(program, arguments);
}

void Process::jar(const QString &jar, const QStringList &jarArguments)
{
    QStringList arguments;
    const int minHeapSize = app->settings->getJavaMinHeapSize();
    const int maxHeapSize = app->settings->getJavaMaxHeapSize();
    if (app->settings->getJavaMinHeapSize()) {
        arguments << QString("-Xms%1m").arg(minHeapSize);
    }
    if (app->settings->getJavaMaxHeapSize()) {
        arguments << QString("-Xmx%1m").arg(maxHeapSize);
    }
    arguments << "-jar" << jar << jarArguments;
    run(app->getJavaBinaryPath("java"), arguments);
}
