#ifndef KEYTOOL_H
#define KEYTOOL_H

#include "tools/executable.h"
#include "tools/keystore.h"
#include "base/application.h"

class Keytool : public Executable
{
public:
    explicit Keytool(QObject *parent = nullptr) : Keytool(app->getJavaBinaryPath("keytool"), parent) {}
    explicit Keytool(const QString &executable, QObject *parent = nullptr) : Executable(executable, parent) {}

    Result<QString> create(const Keystore &keystore) const;
    Result<QString> aliases(const QString &keystore, const QString &password) const;

private:
    void normalizeDname(Dname &dname) const;
};

#endif // KEYTOOL_H
