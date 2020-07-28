#ifndef FILEASSOCIATION_H
#define FILEASSOCIATION_H

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <QString>

class QSettings;

class FileAssociation
{
public:
    FileAssociation(const QString &progId, const QString &extension);

    bool set(const QString &icon = QString(), const QString &friendlyTypeName = QString());
    bool unset();

    bool addVerb(const QString &verb, const QString &command, const QString &icon = QString());
    bool removeVerb(const QString &verb);

    bool isSet() const;
    bool hasVerb(const QString &verb) const;

private:
    QString getVerbKey(const QString &verb) const;

    const QString progId;
    const QString extension;
    std::unique_ptr<QSettings> hkcuClasses;
    std::unique_ptr<QSettings> hklmClasses;
};

#endif
#endif // FILEASSOCIATION_H
