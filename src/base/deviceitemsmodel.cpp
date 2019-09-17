#include "base/deviceitemsmodel.h"
#include "base/application.h"
#include "tools/adb.h"

const Device *DeviceItemsModel::get(const QModelIndex &index) const
{
    if (index.isValid()) {
        return devices.at(index.row()).data();
    }
    return nullptr;
}

void DeviceItemsModel::refresh()
{
    Adb adb;
    if (!devices.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
            devices.clear();
        endRemoveRows();
    }
    const QList<QSharedPointer<Device>> list = adb.devices();
    if (!list.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, list.size() - 1);
            for (const QSharedPointer<Device> &device : list) {
                const QString serial = device->getSerial();
                const QString alias = app->settings->getDeviceAlias(serial);
                if (!alias.isEmpty()) {
                    device->setAlias(alias);
                }
            }
            devices.append(list);
        endInsertRows();
    }
}

void DeviceItemsModel::save() const
{
    for (const QSharedPointer<Device> &device : devices) {
        const QString alias = device->getAlias();
        if (!alias.isEmpty()) {
            app->settings->setDeviceAlias(device->getSerial(), alias);
        }
    }
}

bool DeviceItemsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole && index.column() == DeviceAlias) {
        devices[index.row()]->setAlias(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant DeviceItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        auto device = devices.at(index.row());
        switch (index.column()) {
        case DeviceAlias: {
            const QString alias = device->getAlias();
            return alias.isEmpty() ? device->getSerial() : alias;
        }
        case DeviceSerial:
            return device->getSerial();
        case DeviceProduct:
            return device->getProductString();
        case DeviceModel:
            return device->getModelString();
        case DeviceDevice:
            return device->getDeviceString();
        }
    }
    return QVariant();
}

QVariant DeviceItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
                case DeviceAlias:   return tr("Alias");
                case DeviceSerial:  return tr("Serial Number");
                case DeviceProduct: return tr("Product");
                case DeviceModel:   return tr("Model");
                case DeviceDevice:  return tr("Device");
            }
        }
    }
    return QVariant();
}

QModelIndex DeviceItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row >= 0 && row < devices.count()) {
        return createIndex(row, column, devices.at(row).data());
    }
    return QModelIndex();
}

int DeviceItemsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return devices.count();
}

int DeviceItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}
