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

    class Verify : public Command
    {
    public:
        Verify(const QString &apkPath, QObject *parent = nullptr)
            : Command(parent)
            , apkPath(apkPath) {}

        void run() override;
        bool hasV1Scheme() const;
        bool hasV2Scheme() const;
        bool hasV3Scheme() const;
        const QStringList &signersInfo() const;

    private:
        const QString apkPath;
        bool resultV1Scheme{false};
        bool resultV2Scheme{false};
        bool resultV3Scheme{false};
        QStringList resultSignersInfo;
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
