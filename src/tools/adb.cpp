#include "tools/adb.h"
#include "base/process.h"
#include "base/application.h"
#include "base/utils.h"
#include <QDebug>
#include <QRegularExpression>

void Adb::Cd::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "shell" << "cd" << path;

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);

}

const QString &Adb::Cd::output() const
{
    return resultOutput;
}

void Adb::Mkdir::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "shell" << "mkdir" << path;

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Adb::Mkdir::output() const
{
    return resultOutput;
}

void Adb::Cp::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "shell" << "cp" << "-R" << "-n" << src << dst;

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Adb::Cp::output() const
{
    return resultOutput;
}

void Adb::Mv::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "shell" << "mv" << "-n" << src << dst;

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Command::finished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->run(getPath(), arguments);
}

void Adb::Rm::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "shell" << "rm" << "-rf" << path;

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Command::finished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->run(getPath(), arguments);
}

void Adb::Ls::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "shell" << "stat" << "-L" << "-c" << "'%F,%n'" << QString("%1/*").arg(path);

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
        fileSystemItems.clear();
        const auto entries = output.split('\n');
        for (const QString &entry : entries) {
            if (!entry.isEmpty() && !entry.endsWith(": Permission denied")) {
                const QRegularExpression regex("^(.+?),(.+)$");
                const QString fsItemPath = regex.match(entry).captured(2);
                const QString fsItemType = regex.match(entry).captured(1);
                if (fsItemType == "regular file") {
                    fileSystemItems.append(AndroidFileSystemItem(fsItemPath, AndroidFileSystemItem::AndroidFSFile));
                } else if (fsItemType == "directory") {
                    fileSystemItems.append(AndroidFileSystemItem(fsItemPath, AndroidFileSystemItem::AndroidFSDirectory));
                } else {
#ifdef QT_DEBUG
                    qWarning() << "Warning: Unhandled AndroidFileSystemItem type" << fsItemType;
#endif
                }
            }
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QList<AndroidFileSystemItem> &Adb::Ls::getFileSystemItems() const
{
    return fileSystemItems;
}

void Adb::Install::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "install" << "-r" << apk;

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
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

void Adb::Push::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "push" << src << dst;

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Command::finished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->run(getPath(), arguments);
}

void Adb::Pull::run()
{
    emit started();

    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "pull" << src << dst;

    auto process = new Process(this);
    connect(process, &Process::finished, this, &Command::finished);
    connect(process, &Process::finished, process, &QObject::deleteLater);
    process->run(getPath(), arguments);
}

void Adb::Screenshot::run()
{
    QStringList arguments;
    if (!serial.isEmpty()) {
        arguments << "-s" << serial;
    }
    arguments << "exec-out" << "screencap" << "-p";

    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success) {
        if (!success) {
            QFile::remove(dst);
        }
        emit finished(success);
        process->deleteLater();
    });
    process->setStandardOutputFile(dst);
    process->run(getPath(), arguments);
}

void Adb::Devices::run()
{
    emit started();
    auto process = new Process(this);
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
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
                    Device device(serial);
                    device.setModelString(modelString);
                    device.setDeviceString(deviceString);
                    device.setProductString(productString);
                    resultDevices.append(device);
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

const QList<Device> &Adb::Devices::devices() const
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
    connect(process, &Process::finished, this, [=](bool success, const QString &output) {
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
    const QString path = Utils::toAbsolutePath(app->settings->getAdbPath());
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Adb::getDefaultPath()
{
    return Utils::getBinaryPath("adb");
}

QString Adb::escapePath(QString path)
{
    path = QString("\"%1\"").arg(path);
    return path;
}
