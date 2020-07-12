#ifndef KEYSTORE_H
#define KEYSTORE_H

#include <QCoreApplication>
#include <memory>

struct Dname
{
    QString name;
    QString org;
    QString orgUnit;
    QString city;
    QString state;
    QString countryCode;
};

class Keystore
{
    Q_DECLARE_TR_FUNCTIONS(Keystore)

public:
    static std::unique_ptr<const Keystore> get(QWidget *parent = nullptr);
    QString keystorePath;
    QString keystorePassword;
    QString keyAlias;
    QString keyPassword;
    int validity;
    Dname dname;
};

#endif // KEYSTORE_H
