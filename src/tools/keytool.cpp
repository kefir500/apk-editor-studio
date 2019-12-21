#include "tools/keytool.h"
#include "base/process.h"
#include "base/application.h"
#include <QStringList>

void Keytool::createKeystore(const Keystore &keystore)
{
    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            emit keystoreCreated();
        } else {
            if (output.contains(QRegularExpression("alias <.+> already exists"))) {
                emit keystoreCreateError(tr("Could not write to keystore: alias already exists."), output);
            } else {
                emit keystoreCreateError(tr("Could not write to keystore. See details for more information."), output);
            }
        }
        process->deleteLater();
    });

    Dname dname = keystore.dname;
    normalizeDname(dname);
    QString dnameArg = QString("CN=%1,").arg(dname.name);
    if (!dname.orgUnit.isEmpty()) { dnameArg += QString("OU=%1,").arg(dname.orgUnit); }
    if (!dname.org.isEmpty()) { dnameArg += QString("O=%1,").arg(dname.org); }
    if (!dname.city.isEmpty()) { dnameArg += QString("L=%1,").arg(dname.city); }
    if (!dname.state.isEmpty()) { dnameArg += QString("ST=%1,").arg(dname.state); }
    if (!dname.countryCode.isEmpty()) { dnameArg += QString("C=%1,").arg(dname.countryCode); }
    dnameArg.chop(1);

    QStringList arguments;
    arguments << "-genkey";
    arguments << "-keystore" << keystore.keystorePath;
    arguments << "-storepass" << keystore.keystorePassword;
    arguments << "-storetype" << "jks";
    arguments << "-keyalg" << "RSA";
    arguments << "-keysize" << "2048";
    arguments << "-validity" << QString::number(keystore.validity);
    arguments << "-alias" << keystore.keyAlias;
    arguments << "-keypass" << keystore.keyPassword;
    arguments << "-dname" << dnameArg;

    process->run(getPath(), arguments);
}

void Keytool::fetchAliases(const QString &keystore, const QString &password)
{
    QRegularExpression regex("^(.+),.+,\\s*PrivateKeyEntry,\\s*$");
    regex.setPatternOptions(QRegularExpression::MultilineOption);

    auto process = new Process(this);
    connect(process, &Process::finished, [=](bool success, const QString &output) {
        if (success) {
            QStringList aliases;
            QRegularExpressionMatchIterator it = regex.globalMatch(output);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                aliases << match.captured(1);
            }
            emit aliasesFetched(aliases);
        } else {
            if (output.contains("Keystore was tampered with, or password was incorrect")) {
                emit aliasesFetchError(tr("Could not read keystore: incorrect password."), output);
            } else {
                emit aliasesFetchError(tr("Could not read keystore. See details for more information."), output);
            }
        }
        process->deleteLater();
    });

    process->run(getPath(), {"-list", "-keystore", keystore, "-storepass", password});
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

QString Keytool::getPath() const
{
    return app->getJavaBinaryPath("keytool");
}
