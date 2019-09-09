#include "tools/keytool.h"
#include <QStringList>

Result<QString> Keytool::create(const Keystore &keystore) const
{
    QStringList arguments;
    arguments << "-genkey";
    arguments << "-keystore" << keystore.keystorePath;
    arguments << "-storepass" << keystore.keystorePassword;
    arguments << "-keyalg" << "RSA";
    arguments << "-keysize" << "2048";
    arguments << "-validity" << QString::number(keystore.validity);
    arguments << "-alias" << keystore.keyAlias;
    arguments << "-keypass" << keystore.keyPassword;

    Dname dname = keystore.dname;
    normalizeDname(dname);
    QString dnameArg = QString("CN=%1,").arg(dname.name);
    if (!dname.orgUnit.isEmpty()) { dnameArg += QString("OU=%1,").arg(dname.orgUnit); }
    if (!dname.org.isEmpty()) { dnameArg += QString("O=%1,").arg(dname.org); }
    if (!dname.city.isEmpty()) { dnameArg += QString("L=%1,").arg(dname.city); }
    if (!dname.state.isEmpty()) { dnameArg += QString("ST=%1,").arg(dname.state); }
    if (!dname.countryCode.isEmpty()) { dnameArg += QString("C=%1,").arg(dname.countryCode); }
    dnameArg.chop(1);

    arguments << "-dname" << dnameArg;

    return startSync(arguments);
}

Result<QString> Keytool::aliases(const QString &keystore, const QString &password) const
{
    QStringList arguments;
    arguments << "-list";
    arguments << "-keystore" << keystore;
    arguments << "-storepass" << password;
    return startSync(arguments);
}

void Keytool::normalizeDname(Dname &dname) const
{
    dname.name.replace(',', "\\,");
    dname.orgUnit.replace(',', "\\,");
    dname.org.replace(',', "\\,");
    dname.city.replace(',', "\\,");
    dname.state.replace(',', "\\,");
    dname.countryCode.replace(',', "\\,");
}
