#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "base/deviceitemsmodel.h"
#include <QDialog>
#include <QLabel>
#include <QListView>
#include <QDialogButtonBox>

class DeviceManager : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceManager(QWidget *parent = nullptr);

    void refreshDevices();
    const Device *getTargetDevice();

private:
    bool setCurrentDevice(const Device *device);

    QListView *deviceList;
    DeviceItemsModel deviceModel;

    QLineEdit *fieldAlias;
    QLabel *labelSerial;
    QLabel *labelProduct;
    QLabel *labelModel;
    QLabel *labelDevice;

    QDialogButtonBox *dialogButtons;
};

#endif // DEVICEMANAGER_H
