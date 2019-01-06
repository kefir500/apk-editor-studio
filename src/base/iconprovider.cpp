#include "base/iconprovider.h"
#include "base/application.h"
#include <QDirIterator>

QIcon IconProvider::get(const QString &name)
{
    // Find existing icon in a pool:
    auto existing = pool.value(name);
    if (!existing.isNull()) {
        return existing;
    }

    // Add new icon to a pool:
    QIcon icon;
    QDirIterator it(app->getSharedPath("resources/icons"), {name}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        icon.addFile(it.next());
    }
    if (!icon.isNull()) {
        pool.insert(name, icon);
    }
    return icon;
}
