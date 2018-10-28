#ifndef DEVICESMODEL_H
#define DEVICESMODEL_H

#include "base/device.h"
#include <QAbstractTableModel>

class DevicesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum DeviceColumn {
        DeviceAlias,
        DeviceSerial,
        DeviceProduct,
        DeviceModel,
        DeviceDevice,
        ColumnCount
    };

    const Device *get(const QModelIndex &index) const;
    void refresh();
    void save();

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    QList<QSharedPointer<Device> > devices;
};

#endif // DEVICESMODEL_H
