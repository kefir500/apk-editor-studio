#ifndef DEVICE_H
#define DEVICE_H

#include <QSharedData>

class Device
{
public:
    Device() : d(nullptr) {}
    Device(const QString &serial);

    const QString &getSerial() const;
    const QString &getAlias() const;
    const QString &getProductString() const;
    const QString &getModelString() const;
    const QString &getDeviceString() const;

    bool isNull() const;

    void setAlias(const QString &alias);
    void setProductString(const QString &product);
    void setModelString(const QString &model);
    void setDeviceString(const QString &device);

private:
    struct DevicePrivate : public QSharedData
    {
        QString alias;
        QString serial;
        QString product;
        QString model;
        QString device;
    };

    QSharedDataPointer<DevicePrivate> d;
};

#endif // DEVICE_H
