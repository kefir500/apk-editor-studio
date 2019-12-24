#ifndef PROJECTSTATE_H
#define PROJECTSTATE_H

#include <QObject>

class ProjectState : public QObject
{
    Q_OBJECT

public:
    enum State {
        StateNormal,
        StateErrored,
        StateUnpacking,
        StatePacking,
        StateSigning,
        StateOptimizing,
        StateInstalling
    };

    ProjectState();

    void setCurrentState(const State &state);
    void setUnpacked(bool unpacked);
    void setModified(bool modified);

    const State &getCurrentState() const;
    bool isUnpacked() const;
    bool isModified() const;
    bool isIdle() const;

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
    State state;
};

#endif // PROJECTSTATE_H
