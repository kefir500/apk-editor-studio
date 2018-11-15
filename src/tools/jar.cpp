#include "tools/jar.h"
#include <QStringList>

Jar::Jar(const QString &jar, QObject *parent) : Java(parent)
{
    this->jar = jar;
}

void Jar::startAsync(const QStringList &options)
{
    QStringList arguments;
    arguments << "-jar" << jar << options;
    Executable::startAsync(arguments);
}

Result<QString> Jar::startSync(const QStringList &options) const
{
    QStringList arguments;
    arguments << "-jar" << jar << options;
    return Executable::startSync(arguments);
}
