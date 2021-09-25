#ifndef PACKAGELIST_H
#define PACKAGELIST_H

#include <QComboBox>

class Package;

class PackageList : public QComboBox
{
    Q_OBJECT

public:
    explicit PackageList(QWidget *parent = nullptr);
    bool setCurrentPackage(Package *package);

signals:
    void currentPackageChanged(Package *package);
};

#endif // PACKAGELIST_H
