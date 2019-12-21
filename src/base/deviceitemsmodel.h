#ifndef DEVICEITEMSMODEL_H
#define DEVICEITEMSMODEL_H

#include "base/device.h"
#include <QAbstractTableModel>
#include <QSharedPointer>

class DeviceItemsModel : public QAbstractTableModel
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
    void save() const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

signals:
    void fetching() const;
    void fetched() const;

private:
    QList<QSharedPointer<Device>> devices;
};

#endif // DEVICEITEMSMODEL_H
