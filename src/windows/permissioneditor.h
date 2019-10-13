#ifndef PERMISSIONEDITOR_H
#define PERMISSIONEDITOR_H

#include "apk/manifest.h"
#include <QGridLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDialog>

class PermissionEditor : public QDialog
{
    Q_OBJECT

public:
    PermissionEditor(Manifest *manifest, QWidget *parent = nullptr);

private:
    void addPermissionLine(const Permission &permission);

    Manifest *manifest;
    QGridLayout *grid;
    QComboBox *comboAdd;
    QPushButton *btnAdd;
    QDialogButtonBox *buttons;
};

#endif // PERMISSIONEDITOR_H
