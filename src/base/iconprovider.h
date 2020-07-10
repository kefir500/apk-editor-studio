#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QIcon>
#include <QFileIconProvider>

class IconProvider
{
public:
    QIcon get(const QFileInfo &fileInfo) const;
    QIcon get(QFileIconProvider::IconType iconType) const;

private:
    QFileIconProvider fileIconProvider;
};

#endif // ICONPROVIDER_H
