#ifndef ADB_H
#define ADB_H

#include "tools/executable.h"
#include "base/device.h"

class Adb : public Executable
{
public:
    explicit Adb(const QString &executable, QObject *parent = nullptr);

    void install(const QString &apk, const QString &serial = QString());
    QList<QSharedPointer<Device> > devices() const;
    QString version() const;
};

#endif // ADB_H
