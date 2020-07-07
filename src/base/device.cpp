#include "base/device.h"

Device::Device(const QString &serial)
{
    d = new DevicePrivate;
    d->serial = serial;
}

const QString &Device::getSerial() const
{
    return d->serial;
}

const QString &Device::getAlias() const
{
    return d->alias;
}

const QString &Device::getProductString() const
{
    return d->product;
}

const QString &Device::getModelString() const
{
    return d->model;
}

const QString &Device::getDeviceString() const
{
    return d->device;
}

bool Device::isNull() const
{
    return !d || d->serial.isNull();
}

void Device::setAlias(const QString &alias)
{
    d->alias = alias;
}

void Device::setProductString(const QString &product)
{
    d->product = product;
}

void Device::setModelString(const QString &model)
{
    d->model = model;
}

void Device::setDeviceString(const QString &device)
{
    d->device = device;
}
