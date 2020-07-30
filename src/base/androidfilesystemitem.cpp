#include "base/androidfilesystemitem.h"
#include <QFileIconProvider>

AndroidFileSystemItem::AndroidFileSystemItem(const QString &path, Type type)
    : path(path)
    , name(QFileInfo(path).fileName())
    , type(type)
{
}

QString AndroidFileSystemItem::getPath() const
{
    return path;
}

QString AndroidFileSystemItem::getName() const
{
    return name;
}

QIcon AndroidFileSystemItem::getIcon(const QFileIconProvider &iconProvider) const
{
    switch (type) {
    case AndroidFSFile:
        return iconProvider.icon(name.toLower());
    case AndroidFSDirectory:
        return iconProvider.icon(QFileIconProvider::Folder);
    default:
        return QIcon();
    }
}

AndroidFileSystemItem::Type AndroidFileSystemItem::getType() const
{
    return type;
}
