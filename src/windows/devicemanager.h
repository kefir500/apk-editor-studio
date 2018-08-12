#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "base/devicesmodel.h"
#include <QDialog>
#include <QLabel>
#include <QListView>
#include <QDialogButtonBox>

class DeviceManager : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceManager(QWidget *parent = 0);

    void refreshDevices();
    const Device *getDevice();

private:
    bool setCurrentDevice(const Device *device);

    QListView *deviceList;
    DevicesModel deviceModel;

    QLineEdit *fieldAlias;
    QLabel *labelSerial;
    QLabel *labelProduct;
    QLabel *labelModel;
    QLabel *labelDevice;

    QDialogButtonBox *dialogButtons;
};

#endif // DEVICEMANAGER_H
