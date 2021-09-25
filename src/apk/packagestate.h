#ifndef PACKAGESTATE_H
#define PACKAGESTATE_H

#include <QObject>

class PackageState : public QObject
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

    PackageState();

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
    void changed();

private:
    bool unpacked;
    bool modified;
    Status status;
};

#endif // PACKAGESTATE_H
