#ifndef PROJECTACTIONVIEWER_H
#define PROJECTACTIONVIEWER_H

#include "editors/actionviewer.h"
#include "apk/project.h"

class ProjectActionViewer : public ActionViewer
{
    Q_OBJECT

public:
    explicit ProjectActionViewer(Project *project, QWidget *parent = nullptr);

signals:
    void apkSaveRequested() const;
    void apkInstallRequested() const;
    void titleEditorRequested() const;

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

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

#endif // PROJECTACTIONVIEWER_H
