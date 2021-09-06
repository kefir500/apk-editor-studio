#ifndef PROJECTSHEET_H
#define PROJECTSHEET_H

#include "sheets/baseactionsheet.h"
#include "apk/project.h"

class ProjectSheet : public BaseActionSheet
{
    Q_OBJECT

public:
    explicit ProjectSheet(Project *project, QWidget *parent = nullptr);

signals:
    void apkSaveRequested();
    void apkInstallRequested();
    void titleEditorRequested();

protected:
    void changeEvent(QEvent *event) override;

private:
    void onProjectUpdated();
    void retranslate();

    Project *project;

    QPushButton *btnEditIcon;
    QPushButton *btnEditTitle;
    QPushButton *btnExplore;
    QPushButton *btnSave;
    QPushButton *btnInstall;
};

#endif // PROJECTSHEET_H
