#ifndef KEYTOOL_H
#define KEYTOOL_H

#include "tools/keystore.h"
#include <QObject>

class Keytool : public QObject
{
    Q_OBJECT

public:
    explicit Keytool(QObject *parent = nullptr) : QObject(parent) {}

    void createKeystore(const Keystore &keystore);
    void fetchAliases(const QString &keystore, const QString &password);

signals:
    void keystoreCreated();
    void keystoreCreateError(const QString &brief, const QString &detailed);
    void aliasesFetched(const QStringList &aliases) const;
    void aliasesFetchError(const QString &brief, const QString &detailed) const;

private:
    void normalizeDname(Dname &dname) const;
    QString getPath() const;
};

#endif // KEYTOOL_H
