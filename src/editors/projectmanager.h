#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "editors/quicktab.h"
#include "apk/project.h"

class ProjectManager : public QuickTab
{
    Q_OBJECT

public:
    explicit ProjectManager(Project *project, QWidget *parent = nullptr);

    void setSaveButtonEnabled(bool enabled);
    void setInstallButtonEnabled(bool enabled);

signals:
    void apkSaveRequested() const;
    void apkInstallRequested() const;
    void titleEditorRequested() const;

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    void retranslate();

    Project *project;

    QPushButton *btnEditIcon;
    QPushButton *btnEditTitle;
    QPushButton *btnSave;
    QPushButton *btnInstall;
};

#endif // PROJECTMANAGER_H
