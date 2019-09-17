#include "tools/zipalign.h"
#include "base/application.h"
#include <QFile>
#include <QStringList>

void Zipalign::align(const QString &apk)
{
    const QString tempApk = apk + ".aligned";

    QStringList arguments;
    arguments << "-f";
    arguments << "4";
    arguments << apk;
    arguments << tempApk;

    disconnect(this, &Zipalign::success, this, nullptr);
    connect(this, &Zipalign::success, [=]() {
        QFile::remove(apk);
        QFile::rename(tempApk, apk);
    });

    Executable::startAsync(arguments);
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
