#include "base/fileassociation.h"

#ifdef Q_OS_WIN

#include <QSettings>
#include <shlobj.h>

FileAssociation::FileAssociation(const QString &progId, const QString &extension)
    : progId(progId)
    , extension(extension)
    , hkcuClasses(new QSettings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat))
    , hklmClasses(new QSettings("HKEY_LOCAL_MACHINE\\Software\\Classes", QSettings::NativeFormat))
{
}

bool FileAssociation::set(const QString &icon, const QString &friendlyTypeName)
{
    if (!hkcuClasses->isWritable()) {
        return false;
    }
    hkcuClasses->setValue(QString(".%1/Default").arg(extension), progId);
    hkcuClasses->setValue(QString(".%1/OpenWithProgIds/%2").arg(extension, progId), QString());
    hkcuClasses->setValue(progId + "/FriendlyTypeName", friendlyTypeName);
    if (!icon.isNull()) {
        hkcuClasses->setValue(progId + "/DefaultIcon/Default", icon);
    }
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
    return true;
}

bool FileAssociation::unset()
{
    if (!hkcuClasses->isWritable()) {
        return false;
    }
    const QString valueKey = QString(".%1/Default").arg(extension);
    if (hkcuClasses->value(valueKey) == progId) {
        hkcuClasses->setValue(valueKey, QString());
    }
    hkcuClasses->remove(QString(".%1/OpenWithProgIds/%2").arg(extension, progId));
    hkcuClasses->remove(progId);
    return true;
}

bool FileAssociation::addVerb(const QString &verb, const QString &command, const QString &icon)
{
    if (!hkcuClasses->isWritable()) {
        return false;
    }
    hkcuClasses->setValue(getVerbKey(verb) + "/Command/Default", command);
    if (!icon.isNull()) {
        hkcuClasses->setValue(getVerbKey(verb) + "/Icon", icon);
    }
    return true;
}

bool FileAssociation::removeVerb(const QString &verb)
{
    if (!hkcuClasses->isWritable()) {
        return false;
    }
    hkcuClasses->remove(getVerbKey(verb));
    return true;
}

bool FileAssociation::isSet() const
{
    auto f = [this](const QSettings *classes) -> bool {
        // Qt doesn't check the existence of a registry key with no value.
        // For this reason the FriendlyTypeName key is checked instead.
        return
            classes->contains(progId + "/FriendlyTypeName") &&
            classes->value(QString(".%1/Default").arg(extension)) == progId;
    };
    return f(hkcuClasses.get()) || f(hklmClasses.get());
}

bool FileAssociation::hasVerb(const QString &verb) const
{
    return
        hkcuClasses->contains(getVerbKey(verb) + "/Command/Default") ||
        hklmClasses->contains(getVerbKey(verb) + "/Command/Default");
}

QString FileAssociation::getVerbKey(const QString &verb) const
{
    return QString("%1/Shell/%2").arg(progId, verb);
}

#endif
