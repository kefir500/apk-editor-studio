#include "tools/apksigner.h"
#include <QStringList>

void Apksigner::sign(const QString &target, const Keystore *keystore)
{
    return sign(target, keystore->keystorePath, keystore->keystorePassword, keystore->keyAlias, keystore->keyPassword);
}

void Apksigner::sign(const QString &target, const QString &keystorePath, const QString &keystorePassword, const QString &keyAlias, const QString &keyPassword)
{
    if (target.isEmpty()) {
        emit error("Apksigner: Target path not specified.");
        return;
    }

    QStringList arguments;
    arguments << "sign";
    arguments << "--ks" << keystorePath;
    arguments << "--ks-key-alias" << keyAlias;
    arguments << "--ks-pass" << QString("pass:%1").arg(keystorePassword);
    arguments << "--key-pass" << QString("pass:%1").arg(keyPassword);
    arguments << target;

    Jar::startAsync(arguments);
}

QString Apksigner::version() const
{
    QStringList arguments;
    arguments << "--version";
    auto result = startSync(arguments);
    return result.success ? result.value : QString();
}
