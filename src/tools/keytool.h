#ifndef KEYTOOL_H
#define KEYTOOL_H

#include "tools/command.h"
#include "tools/keystore.h"

namespace Keytool
{
    class Genkey : public Command
    {
        Q_OBJECT

    public:
        enum ErrorType {
            AliasExistsError,
            UnknownError
        };

        Genkey(const Keystore &keystore, QObject *parent = nullptr)
            : Command(parent)
            , keystore(keystore) {}

        void run() override;

    signals:
        void success() const;
        void error(ErrorType errorType, const QString &brief, const QString &detailed) const;

    private:
        const Keystore keystore;
    };

    class Aliases : public Command
    {
        Q_OBJECT

    public:
        enum ErrorType {
            IncorrectPasswordError,
            UnknownError
        };

        Aliases(const QString &keystore, const QString &password, QObject *parent = nullptr)
            : Command(parent)
            , keystore(keystore)
            , password(password) {}

        void run() override;

    signals:
        void success(const QStringList &aliases) const;
        void error(ErrorType errorType, const QString &brief, const QString &detailed) const;

    private:
        const QString keystore;
        const QString password;
    };

    void normalizeDname(Dname &dname);
    QString getPath();
}

#endif // KEYTOOL_H
