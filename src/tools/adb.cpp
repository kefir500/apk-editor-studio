#include "tools/adb.h"
#include "base/process.h"
#include "base/application.h"
#include "windows/dialogs.h"
#include <QSharedPointer>
#include <QRegularExpression>

void Adb::Install::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "install" << "-r" << apk;

    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Adb::Install::output() const
{
    return resultOutput;
}

void Adb::Devices::run()
{
    emit started();
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        resultDevices.clear();
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
                    resultDevices.append(QSharedPointer<Device>(device));
                }
            }
        } else {
            resultError = output;
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), {"devices", "-l"});
}

const QList<QSharedPointer<Device>> &Adb::Devices::devices() const
{
    return resultDevices;
}

const QString &Adb::Devices::error() const
{
    return resultError;
}

void Adb::Version::run()
{
    emit started();
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            QRegularExpression regex("Android Debug Bridge version (.+)");
            resultVersion = regex.match(output).captured(1).trimmed();
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), {"version"});
}

const QString &Adb::Version::version() const
{
    return resultVersion;
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
