#include "base/fileassociation.h"
#include <QSettings>

#ifdef Q_OS_WIN

FileAssociation::FileAssociation(const QString &progId, const QString &extension)
    : progId(progId)
    , extension(extension)
    , registry(new QSettings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat))
{
}

bool FileAssociation::set(const QString &icon, const QString &friendlyTypeName)
{
    if (!registry->isWritable()) {
        return false;
    }
    registry->setValue(QString(".%1/Default").arg(extension), progId);
    registry->setValue(QString(".%1/OpenWithProgIds/%2").arg(extension, progId), QString());
    registry->setValue(progId + "/FriendlyTypeName", friendlyTypeName);
    if (!icon.isNull()) {
        registry->setValue(progId + "/DefaultIcon/Default", icon);
    }
    return true;
}

bool FileAssociation::unset()
{
    if (!registry->isWritable()) {
        return false;
    }
    const QString valueKey = QString(".%1/Default").arg(extension);
    if (registry->value(valueKey) == progId) {
        registry->setValue(valueKey, QString());
    }
    registry->remove(QString(".%1/OpenWithProgIds/%2").arg(extension, progId));
    registry->remove(progId);
    return true;
}

bool FileAssociation::addVerb(const QString &verb, const QString &command, const QString &icon)
{
    if (!registry->isWritable()) {
        return false;
    }
    registry->setValue(getVerbKey(verb) + "/Command/Default", command);
    if (!icon.isNull()) {
        registry->setValue(getVerbKey(verb) + "/Icon", icon);
    }
    return true;
}

bool FileAssociation::removeVerb(const QString &verb)
{
    if (!registry->isWritable()) {
        return false;
    }
    registry->remove(getVerbKey(verb));
    return true;
}

bool FileAssociation::isSet() const
{
    // Qt doesn't check the existence of a registry key with no value.
    // For this reason the FriendlyTypeName key is checked instead.
    return
        registry->contains(progId + "/FriendlyTypeName") &&
        registry->value(QString(".%1/Default").arg(extension)) == progId;
}

bool FileAssociation::hasVerb(const QString &verb) const
{
    return registry->contains(getVerbKey(verb) + "/Command/Default");
}

QString FileAssociation::getVerbKey(const QString &verb) const
{
    return QString("%1/Shell/%2").arg(progId, verb);
}

#endif
