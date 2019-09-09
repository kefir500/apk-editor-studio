#ifndef KEYSTORE_H
#define KEYSTORE_H

#include <QString>

struct Dname
{
    QString name;
    QString org;
    QString orgUnit;
    QString city;
    QString state;
    QString countryCode;
};

struct Keystore
{
    QString keystorePath;
    QString keystorePassword;
    QString keyAlias;
    QString keyPassword;
    int validity;
    Dname dname;
};

#endif // KEYSTORE_H
