#ifndef APKSIGNER_H
#define APKSIGNER_H

#include "tools/command.h"
#include "tools/keystore.h"

namespace Apksigner
{
    class Sign : public Command
    {
        Q_OBJECT

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

    signals:
        void finished(bool success, const QString &output) const;

    private:
        const QString target;
        const QString keystorePath;
        const QString keystorePassword;
        const QString keyAlias;
        const QString keyPassword;
    };

    class Version : public Command
    {
        Q_OBJECT

    public:
        Version(QObject *parent = nullptr) : Command(parent) {}
        void run() override;

    signals:
        void finished(const QString &version) const;
    };

    QString getPath();
    QString getDefaultPath();
}


#endif // APKSIGNER_H
