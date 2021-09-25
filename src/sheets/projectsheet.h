#ifndef PROJECTSHEET_H
#define PROJECTSHEET_H

#include "sheets/baseactionsheet.h"

class Package;

class ProjectSheet : public BaseActionSheet
{
    Q_OBJECT

public:
    explicit ProjectSheet(Package *package, QWidget *parent = nullptr);

signals:
    void apkSaveRequested();
    void apkInstallRequested();
    void titleEditorRequested();

protected:
    void changeEvent(QEvent *event) override;

private:
    void onPackageUpdated();
    void retranslate();

    Package *package;

    QPushButton *btnEditIcon;
    QPushButton *btnEditTitle;
    QPushButton *btnExplore;
    QPushButton *btnSave;
    QPushButton *btnInstall;
};

#endif // PROJECTSHEET_H
