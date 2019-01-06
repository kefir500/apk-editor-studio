#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QMap>
#include <QIcon>

class IconProvider
{
public:
    QIcon get(const QString &name);

private:
    QMap<QString, QIcon> pool;
};

#endif // ICONPROVIDER_H
