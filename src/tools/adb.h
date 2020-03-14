#ifndef ADB_H
#define ADB_H

#include "base/androidfilesystemitem.h"
#include "base/command.h"
#include "base/device.h"

namespace Adb
{
    QString escapePath(QString path);

    // Cd

    class Cd : public Command
    {
    public:
        Cd(const QString &path, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , path(escapePath(path))
            , serial(serial) {}

        void run() override;
        const QString &output() const;

    private:
        const QString path;
        const QString serial;
        QString resultOutput;
    };

    // Mkdir

    class Mkdir : public Command
    {
    public:
        Mkdir(const QString &path, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , path(escapePath(path))
            , serial(serial) {}

        void run() override;
        const QString &output() const;

    private:
        const QString path;
        const QString serial;
        QString resultOutput;
    };

    // Cp

    class Cp : public Command
    {
    public:
        Cp(const QString &src, const QString &dst, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , src(escapePath(src))
            , dst(escapePath(dst))
            , serial(serial) {}

        void run() override;
        const QString &output() const;

    private:
        const QString src;
        const QString dst;
        const QString serial;
        QString resultOutput;
    };

    // Mv

    class Mv : public Command
    {
    public:
        Mv(const QString &src, const QString &dst, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , src(escapePath(src))
            , dst(escapePath(dst))
            , serial(serial) {}

        void run() override;

    private:
        const QString src;
        const QString dst;
        const QString serial;
    };

    // Rm

    class Rm : public Command
    {
    public:
        Rm(const QString &path, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , path(escapePath(path))
            , serial(serial) {}

        void run() override;

    private:
        const QString path;
        const QString serial;
    };

    // Ls

    class Ls : public Command
    {
    public:
        Ls(const QString &path, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , path(escapePath(path))
            , serial(serial) {}

        void run() override;
        const QList<AndroidFileSystemItem> &getFileSystemItems() const;

    private:
        const QString path;
        const QString serial;
        QList<AndroidFileSystemItem> fileSystemItems;
    };

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

    // Push

    class Push : public Command
    {
    public:
        Push(const QString &src, const QString &dst, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , src(src)
            , dst(dst)
            , serial(serial) {}

        void run() override;

    private:
        const QString src;
        const QString dst;
        const QString serial;
    };

    // Pull

    class Pull : public Command
    {
    public:
        Pull(const QString &src, const QString &dst, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , src(src)
            , dst(dst)
            , serial(serial) {}

        void run() override;

    private:
        const QString src;
        const QString dst;
        const QString serial;
    };

    // Screenshot

    class Screenshot : public Command
    {
    public:
        Screenshot(const QString &dst, const QString &serial = QString(), QObject *parent = nullptr)
            : Command(parent)
            , dst(dst)
            , serial(serial) {}

        void run() override;

    private:
        const QString dst;
        const QString serial;
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
}

#endif // ADB_H
