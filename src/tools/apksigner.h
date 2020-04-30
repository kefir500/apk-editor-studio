#ifndef APKSIGNER_H
#define APKSIGNER_H

#include "base/command.h"
#include "tools/keystore.h"

namespace Apksigner
{
    class Sign : public Command
    {
    public:
        Sign(const QString &target, const Keystore *keystore, QObject *parent = nullptr)
            : Command(parent)
            , target(target)
            , keystorePath(keystore->keystorePath)
            , keystorePassword(keystore->keystorePassword)
            , keyAlias(keystore->keyAlias)
            , keyPassword(keystore->keyPassword) {}

        Sign(const QString &target, const QString &keystorePath, const QString &keystorePassword,
             const QString &keyAlias, const QString &keyPassword, QObject *parent = nullptr)
            : Command(parent)
            , target(target)
            , keystorePath(keystorePath)
            , keystorePassword(keystorePassword)
            , keyAlias(keyAlias)
            , keyPassword(keyPassword) {}

        void run() override;
        const QString &output() const;

    private:
        const QString target;
        const QString keystorePath;
        const QString keystorePassword;
        const QString keyAlias;
        const QString keyPassword;
        QString resultOutput;
    };

    class Version : public Command
    {
    public:
        Version(QObject *parent = nullptr) : Command(parent) {}
        void run() override;
        const QString &version() const;

    private:
        QString resultVersion;
    };

    QString getPath();
    QString getDefaultPath();
}


#endif // APKSIGNER_H
