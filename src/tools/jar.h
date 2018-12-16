#ifndef JAR_H
#define JAR_H

#include "tools/java.h"

class Jar : public Java
{
public:
    explicit Jar(const QString &jar, QObject *parent = nullptr);

    void startAsync(const QStringList &options) override;
    Result<QString> startSync(const QStringList &options) const override;

private:
    QString jar;
};

#endif // JAR_H
