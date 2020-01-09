#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QMap>
#include <QIcon>
#include <QFileInfo>
#include <QFileIconProvider>

class IconProvider
{
public:
    QIcon get(const QString &iconName);
    QIcon get(const QFileInfo &fileInfo) const;
    QIcon get(QFileIconProvider::IconType iconType) const;

private:
    QMap<QString, QIcon> pool;
    QFileIconProvider fileIconProvider;
};

#endif // ICONPROVIDER_H
