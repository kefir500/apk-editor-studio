#ifndef FILEASSOCIATION_H
#define FILEASSOCIATION_H

#include <QString>

class QSettings;

class FileAssociation
{
public:
    FileAssociation(const QString &progId, const QString &extension);

    bool set(const QString &command, const QString &icon);
    bool unset();
    bool isSet() const;

private:
    const QString progId;
    const QString extension;
    std::unique_ptr<QSettings> registry;
};

#endif // FILEASSOCIATION_H
