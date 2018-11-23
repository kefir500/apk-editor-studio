#ifndef PROJECTVIEWER_H
#define PROJECTVIEWER_H

#include "editors/actionviewer.h"
#include "apk/project.h"

class ProjectViewer : public ActionViewer
{
    Q_OBJECT

public:
    explicit ProjectViewer(Project *project, QWidget *parent = nullptr);

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
    QPushButton *btnExplore;
    QPushButton *btnSave;
    QPushButton *btnInstall;
};

#endif // PROJECTVIEWER_H
