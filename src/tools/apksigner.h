#ifndef APKSIGNER_H
#define APKSIGNER_H

#include "tools/jar.h"
#include "tools/keystore.h"

class Apksigner : public Jar
{
public:
    explicit Apksigner(const QString &jar, QObject *parent = nullptr) : Jar(jar, parent) {}

    void sign(const QString &target, const Keystore *keystore);
    void sign(const QString &target, const QString &keystorePath, const QString &keystorePassword, const QString &keyAlias, const QString &keyPassword);
    QString version() const;
};

#endif // APKSIGNER_H
