#ifndef PROJECTSTATE_H
#define PROJECTSTATE_H

#include <QObject>

class ProjectState : public QObject
{
    Q_OBJECT

public:
    enum ProjectAction {
        ProjectIdle,
        ProjectUnpacking,
        ProjectPacking,
        ProjectSigning,
        ProjectOptimizing,
        ProjectInstalling
    };

    ProjectState();

    void setCurrentAction(ProjectAction action);
    void setUnpacked(bool unpacked);
    void setModified(bool modified);
    void setLastActionFailed(bool fail);

    ProjectAction getCurrentAction() const;
    bool isUnpacked() const;
    bool isModified() const;
    bool isLastActionFailed() const;

    bool canEdit() const;
    bool canSave() const;
    bool canInstall() const;
    bool canExplore() const;
    bool canClose() const;

signals:
    void changed() const;

private:
    bool unpacked;
    bool modified;
    bool lastActionFailed;
    ProjectAction currentAction;
};

#endif // PROJECTSTATE_H
