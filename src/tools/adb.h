#ifndef ADB_H
#define ADB_H

#include "tools/command.h"
#include "base/androidfilesystemitem.h"
#include "base/device.h"

namespace Adb
{
    // Install

    class Install : public Command
    {
        Q_OBJECT

    public:
        Install(const QString &apk, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , apk(apk)
            , serial(serial) {}

        void run() override;

    signals:
        void finished(bool success, const QString &output) const;

    private:
        const QString apk;
        const QString serial;
    };

    // Devices

    class Devices : public Command
    {
        Q_OBJECT

    public:
        Devices(QObject *parent) : Command(parent) {}
        void run() override;

    signals:
        void finished(bool success, QList<QSharedPointer<Device>> devices) const;
    };

    // Version

    class Version : public Command
    {
        Q_OBJECT

    public:
        Version(QObject *parent) : Command(parent) {}
        void run() override;

    signals:
        void finished(const QString &version) const;
    };

    QString getPath();
    QString getDefaultPath();
    QString escapePath(const QString &path);
}

#endif // ADB_H
