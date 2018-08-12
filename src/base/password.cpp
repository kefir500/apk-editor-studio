#include "base/password.h"
#include "qtkeychain/keychain.h"
#include <QEventLoop>
#include <QDebug>

QString Password::get()
{
    QKeychain::ReadPasswordJob password("APK Editor Studio");
    password.setAutoDelete(false);
    password.setKey(key);

    QEventLoop loop;
    password.connect(&password, SIGNAL(finished(QKeychain::Job *)), &loop, SLOT(quit()));

    password.start();
    loop.exec();
    if (password.error()) {
        qWarning() << qPrintable(QString("Could not get secret \"%1\": %2").arg(key, password.errorString()));
    }
    return password.textData();
}

void Password::set(const QString &value)
{
    QKeychain::WritePasswordJob password("APK Editor Studio");
    password.setAutoDelete(false);
    password.setKey(key);
    password.setTextData(value);

    QEventLoop loop;
    password.connect(&password, SIGNAL(finished(QKeychain::Job *)), &loop, SLOT(quit()));

    password.start();
    loop.exec();
    if (password.error()) {
        qWarning() << qPrintable(QString("Could not set secret \"%1\": %2").arg(key, password.errorString()));
    }
}

void Password::remove()
{
    QKeychain::DeletePasswordJob password("APK Editor Studio");
    password.setAutoDelete(false);
    password.setKey(key);

    QEventLoop loop;
    password.connect(&password, SIGNAL(finished(QKeychain::Job *)), &loop, SLOT(quit()));

    password.start();
    loop.exec();
    if (password.error()) {
        qWarning() << qPrintable(QString("Could not delete secret \"%1\": %2").arg(key, password.errorString()));
    }
}
