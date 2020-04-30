#include "base/androidfilesystemitem.h"
#include "base/application.h"

AndroidFileSystemItem::AndroidFileSystemItem(const QString &path, Type type)
    : path(path)
    , name(QFileInfo(path).fileName())
    , type(type)
{
    switch (type) {
    case AndroidFSFile:
        icon = app->icons.get(QFileInfo(this->name.toLower()));
        break;
    case AndroidFSDirectory:
        icon = app->icons.get(QFileIconProvider::Folder);
        break;
    }
}

QString AndroidFileSystemItem::getPath() const
{
    return path;
}

QString AndroidFileSystemItem::getName() const
{
    return name;
}

QIcon AndroidFileSystemItem::getIcon() const
{
    return icon;
}

AndroidFileSystemItem::Type AndroidFileSystemItem::getType() const
{
    return type;
}
