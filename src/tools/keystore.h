#ifndef KEYSTORE_H
#define KEYSTORE_H

#include <QSharedPointer>
#include <QCoreApplication>

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
    static QSharedPointer<const Keystore> get(QWidget *parent = nullptr);
    QString keystorePath;
    QString keystorePassword;
    QString keyAlias;
    QString keyPassword;
    int validity;
    Dname dname;
};

#endif // KEYSTORE_H
