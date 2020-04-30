#include "tools/zipalign.h"
#include "base/process.h"
#include "base/application.h"
#include <QFile>

void Zipalign::Align::run()
{
    emit started();

    const QString tempApk = apk + ".aligned";

    QStringList arguments;
    arguments << "-f";
    arguments << "4";
    arguments << apk;
    arguments << tempApk;

    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            QFile::remove(apk);
            QFile::rename(tempApk, apk);
        }
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Zipalign::Align::output() const
{
    return resultOutput;
}

QString Zipalign::getPath()
{
    const QString path = app->settings->getZipalignPath();
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Zipalign::getDefaultPath()
{
    return app->getBinaryPath("zipalign");
}
