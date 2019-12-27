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
    emit fetching();
    auto adb = new Adb::Devices(this);
    connect(adb, &Adb::Devices::finished, [=](bool success, const QList<QSharedPointer<Device>> &list) {
        if (success) {
            for (const QSharedPointer<Device> &device : list) {
                const QString serial = device->getSerial();
                const QString alias = app->settings->getDeviceAlias(serial);
                if (!alias.isEmpty()) {
                    device->setAlias(alias);
                }
            }
        }
        beginResetModel();
        devices.clear();
        devices.append(list);
        endResetModel();
        emit fetched();
    });
    adb->run();
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
    if (index.isValid() && role == Qt::EditRole && index.column() == AliasColumn) {
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
        case AliasColumn: {
            const QString alias = device->getAlias();
            return alias.isEmpty() ? device->getSerial() : alias;
        }
        case SerialColumn:
            return device->getSerial();
        case ProductColumn:
            return device->getProductString();
        case ModelColumn:
            return device->getModelString();
        case DeviceColumn:
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
            case AliasColumn:
                return tr("Alias");
            case SerialColumn:
                return tr("Serial Number");
            case ProductColumn:
                return tr("Product");
            case ModelColumn:
                return tr("Model");
            case DeviceColumn:
                return tr("Device");
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
