#include "base/iconprovider.h"
#include "base/application.h"
#include <QDirIterator>

QIcon IconProvider::get(const QString &iconName)
{
    // Find existing icon in a pool:
    auto existing = pool.value(iconName);
    if (!existing.isNull()) {
        return existing;
    }

    // Add new icon to a pool:
    QIcon icon;
    QDirIterator it(app->getSharedPath("resources/icons"), {iconName}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        icon.addFile(it.next());
    }
    if (!icon.isNull()) {
        pool.insert(iconName, icon);
    }
    return icon;
}

QIcon IconProvider::get(const QFileInfo &fileInfo) const
{
    return fileIconProvider.icon(fileInfo);
}

QIcon IconProvider::get(QFileIconProvider::IconType iconType) const
{
    return fileIconProvider.icon(iconType);
}
