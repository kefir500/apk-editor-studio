#include "base/iconprovider.h"

QIcon IconProvider::get(const QFileInfo &fileInfo) const
{
    return fileIconProvider.icon(fileInfo.fileName());
}

QIcon IconProvider::get(QFileIconProvider::IconType iconType) const
{
    return fileIconProvider.icon(iconType);
}
