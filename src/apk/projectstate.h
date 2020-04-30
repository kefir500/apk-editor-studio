#ifndef PROJECTSTATE_H
#define PROJECTSTATE_H

#include <QObject>

class ProjectState : public QObject
{
    Q_OBJECT

public:
    enum class Status {
        Normal,
        Errored,
        Unpacking,
        Packing,
        Signing,
        Optimizing,
        Installing
    };

    ProjectState();

    void setCurrentStatus(const Status &status);
    void setUnpacked(bool unpacked);
    void setModified(bool modified);

    const Status &getCurrentStatus() const;
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
    Status status;
};

#endif // PROJECTSTATE_H
