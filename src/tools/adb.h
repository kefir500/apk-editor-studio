#ifndef ADB_H
#define ADB_H

#include "base/device.h"
#include <QObject>

class Adb : public QObject
{
    Q_OBJECT

public:
    explicit Adb(QObject *parent = nullptr);

    void install(const QString &apk, const QString &serial = QString());
    void devices();
    void version();

    static QString getPath();
    static QString getDefaultPath();

signals:
    void installFinished(bool success, const QString &message) const;
    void devicesFetched(bool success, const QList<QSharedPointer<Device>> &devices) const;
    void versionFetched(const QString &version) const;
};

#endif // ADB_H
