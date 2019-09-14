#ifndef PERMISSIONEDITOR_H
#define PERMISSIONEDITOR_H

#include "apk/manifest.h"
#include <QGridLayout>
#include <QDialog>

class PermissionEditor : public QDialog
{
public:
    PermissionEditor(Manifest *manifest, QWidget *parent = nullptr);

private:
    void addPermissionLine(const Permission &permission);

    Manifest *manifest;
    QGridLayout *grid;
};

#endif // PERMISSIONEDITOR_H
