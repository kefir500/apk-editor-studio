#ifndef PERMISSIONEDITOR_H
#define PERMISSIONEDITOR_H

#include "apk/manifest.h"
#include <QDialog>

class PermissionEditor : public QDialog
{
public:
    PermissionEditor(Manifest *manifest, QWidget *parent = nullptr);

private:
    QList<Permission *> forRemoval;
};

#endif // PERMISSIONEDITOR_H
