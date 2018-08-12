#ifndef DEVICE_H
#define DEVICE_H

#include <QString>

class Device {

public:
    Device(const QString &serial);

    QString getSerial() const;
    QString getAlias() const;
    QString getProductString() const;
    QString getModelString() const;
    QString getDeviceString() const;

    void setAlias(const QString &alias);
    void setProductString(const QString &product);
    void setModelString(const QString &model);
    void setDeviceString(const QString &device);

private:
    QString alias;
    QString serial;
    QString product;
    QString model;
    QString device;
};

#endif // DEVICE_H
