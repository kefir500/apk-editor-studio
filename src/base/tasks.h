#ifndef TASKS_H
#define TASKS_H

#include <QObject>
#include <QQueue>
#include "tools/keystore.h"

namespace Tasks
{
    // Task

    class Task : public QObject
    {
        Q_OBJECT
    public:
        Task();
        virtual void run() = 0;
    signals:
        void started() const;
        void finished() const;
        void success() const;
        void error(const QString &message) const;
    protected:
        friend class Batch;
        virtual ~Task() {}
    };

    // Unpack

    class Unpack : public Task
    {
    public:
        Unpack(const QString &source, const QString &target, const QString &frameworks, bool resources, bool sources, bool keepBroken);
        void run() override;
    private:
        QString source;
        QString target;
        QString frameworks;
        bool resources;
        bool sources;
        bool keepBroken;
    };

    // Pack

    class Pack : public Task
    {
    public:
        Pack(const QString &source, const QString &target, const QString &frameworks);
        void run() override;
    private:
        QString source;
        QString target;
        QString frameworks;
    };

    // Zipalign

    class Align : public Task
    {
    public:
        Align(const QString &target);
        void run() override;
    private:
        QString target;
    };

    // Sign

    class Sign : public Task
    {
    public:
        Sign(const QString &target, const Keystore *keystore);
        void run() override;
    private:
        QString target;
        const Keystore *keystore;
    };

    // Install

    class Install : public Task
    {
    public:
        Install(const QString &apk, const QString &serial);
        void run() override;
    private:
        QString apk;
        QString serial;
    };

    // Batch

    class Batch : public Task
    {
        Q_OBJECT
    public:
        void add(Task *task, bool critical = false);
        void run() override;
    private:
        ~Batch() override;
        QQueue<Task *> tasks;
    };
}

#endif // TASKS_H
