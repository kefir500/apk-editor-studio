#include "tools/adb.h"
#include "base/debug.h"
#include <QSharedPointer>
#include <QRegularExpression>

Adb::Adb(const QString &executable, QObject *parent) : Executable(executable, parent)
{
    connect(&process, &QProcess::readyRead, [=]() {
        const QString output = process.readAllStandardOutput();
        if (output.contains("failed to get feature set")) {
            process.kill();
            emit error(output);
        }
    });
}

void Adb::install(const QString &apk, const QString &serial)
{
    QStringList arguments;
    if (!serial.isEmpty()) { arguments << "-s" << serial; }
    arguments << "install" << "-r";
    arguments << apk;
    Executable::startAsync(arguments);
}

QList<QSharedPointer<Device> > Adb::devices() const
{
    QList<QSharedPointer<Device> > list;
    QStringList arguments;
    arguments << "devices" << "-l";
    Result<QString> result = Executable::startSync(arguments);
    if (result.success) {
        QStringList lines = result.value.split('\n');
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
    return list;
}

QString Adb::version() const
{
    QStringList arguments;
    arguments << "version";
    auto result = startSync(arguments);
    if (!result.success) {
        return QString();
    }
    QRegularExpression regex("Android Debug Bridge version (.+)");
    const QString version = regex.match(result.value).captured(1).trimmed();
    return version;
}
