#ifndef APKSIGNER_H
#define APKSIGNER_H

#include "tools/keystore.h"
#include <QObject>

class Apksigner : public QObject
{
    Q_OBJECT

public:
    explicit Apksigner(QObject *parent = nullptr) : QObject( parent) {}

    void sign(const QString &target, const Keystore *keystore);
    void sign(const QString &target, const QString &keystorePath, const QString &keystorePassword, const QString &keyAlias, const QString &keyPassword);
    void version();

    static QString getPath();
    static QString getDefaultPath();

signals:
    void signFinished(bool success, const QString &message) const;
    void versionFetched(const QString &version) const;
};

#endif // APKSIGNER_H
