#include "tools/zipalign.h"
#include "base/process.h"
#include "base/application.h"
#include <QFile>

void Zipalign::align(const QString &apk)
{
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
        emit alignFinished(success, output);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
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
