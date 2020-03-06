#ifndef ADB_H
#define ADB_H

#include "base/command.h"
#include "base/device.h"

namespace Adb
{
    // Install

    class Install : public Command
    {
    public:
        Install(const QString &apk, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , apk(apk)
            , serial(serial) {}

        void run() override;
        const QString &output() const;

    private:
        const QString apk;
        const QString serial;
        QString resultOutput;
    };

    // Devices

    class Devices : public Command
    {
        Q_OBJECT

    public:
        Devices(QObject *parent) : Command(parent) {}
        void run() override;
        const QList<QSharedPointer<Device>> &devices() const;
        const QString &error() const;

    private:
        QList<QSharedPointer<Device>> resultDevices;
        QString resultError;
    };

    // Version

    class Version : public Command
    {
        Q_OBJECT

    public:
        Version(QObject *parent) : Command(parent) {}
        void run() override;
        const QString &version() const;

    private:
        QString resultVersion;
    };

    QString getPath();
    QString getDefaultPath();
    QString escapePath(const QString &path);
}

#endif // ADB_H
