#include "base/device.h"

Device::Device(const QString &serial) : serial(serial)
{
    const QChar dash(0x2013);
    product = dash;
    model = dash;
    device = dash;
}

QString Device::getSerial() const
{
    return serial;
}

QString Device::getAlias() const
{
    return alias;
}

QString Device::getProductString() const
{
    return product;
}

QString Device::getModelString() const
{
    return model;
}

QString Device::getDeviceString() const
{
    return device;
}

void Device::setAlias(const QString &alias)
{
    this->alias = alias;
}

void Device::setProductString(const QString &product)
{
    this->product = product;
}

void Device::setModelString(const QString &model)
{
    this->model = model;
}

void Device::setDeviceString(const QString &device)
{
    this->device = device;
}
