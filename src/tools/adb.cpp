#include "tools/adb.h"
#include "base/process.h"
#include "base/application.h"
#include "windows/dialogs.h"
#include <QSharedPointer>
#include <QRegularExpression>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

Adb::Adb(QObject *parent) : QObject(parent)
{
}

void Adb::install(const QString &apk, const QString &serial)
{
    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "install" << "-r" << apk;

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Adb::installFinished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->run(getPath(), arguments);
}

void Adb::devices()
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        QList<QSharedPointer<Device>> list;
        if (success) {
            QStringList lines = output.split('\n');
            lines.removeFirst();
            for (const QString &line : lines) {
                const QString serial = QRegularExpression("^(\\S+)\\s+device(\\s|$)").match(line).captured(1);
                if (!serial.isEmpty()) {
                    const QString modelString = QRegularExpression("\\s+model:(\\S+)(\\s|$)").match(line).captured(1);
                    const QString deviceString = QRegularExpression("\\s+device:(\\S+)(\\s|$)").match(line).captured(1);
                    const QString productString = QRegularExpression("\\s+product:(\\S+)(\\s|$)").match(line).captured(1);
                    Device *device = new Device(serial);
                    device->setModelString(modelString);
                    device->setDeviceString(deviceString);
                    device->setProductString(productString);
                    list.append(QSharedPointer<Device>(device));
                }
            }
        }
        emit devicesFetched(success, list);
        process->deleteLater();
    });
    process->run(getPath(), {"devices", "-l"});
}

void Adb::version()
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            QRegularExpression regex("Android Debug Bridge version (.+)");
            const QString version = regex.match(output).captured(1).trimmed();
            emit versionFetched(version);
        } else {
            emit versionFetched({});
        }
        process->deleteLater();
    });
    process->run(getPath(), {"version"});
}

QString Adb::getPath()
{
    const QString path = app->settings->getAdbPath();
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Adb::getDefaultPath()
{
    return app->getBinaryPath("adb");
}
