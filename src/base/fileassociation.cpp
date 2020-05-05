#include "base/fileassociation.h"
#include <QSettings>

#ifdef Q_OS_WIN

FileAssociation::FileAssociation(const QString &progId, const QString &extension)
    : progId(progId)
    , extension(extension)
    , registry(new QSettings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat))
{
}

bool FileAssociation::set(const QString &command, const QString &icon)
{
    if (!registry->isWritable()) {
        return false;
    }
    registry->setValue(QString(".%1/Default").arg(extension), progId);
    registry->setValue(QString(".%1/OpenWithProgIds/%2").arg(extension, progId), QString());
    registry->setValue(progId + "/Default", "Android Application Package");
    registry->setValue(progId + "/DefaultIcon/Default", icon);
    registry->setValue(progId + "/Shell/Open/Command/Default", command);
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

bool FileAssociation::isSet() const
{
    return
        registry->value(QString(".%1/Default").arg(extension)) == progId &&
        registry->contains(progId + "/Default");
}

#endif
