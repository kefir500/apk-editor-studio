#include "apk/projectstate.h"

ProjectState::ProjectState()
{
    unpacked = false;
    modified = false;
    status = Status::Normal;
}

void ProjectState::setCurrentStatus(const Status &status)
{
    this->status = status;
    emit changed();
}

void ProjectState::setUnpacked(bool unpacked)
{
    this->unpacked = unpacked;
    emit changed();
}

void ProjectState::setModified(bool modified)
{
    this->modified = modified;
    emit changed();
}

const ProjectState::Status &ProjectState::getCurrentStatus() const
{
    return status;
}

bool ProjectState::isUnpacked() const
{
    return unpacked;
}

bool ProjectState::isModified() const
{
    return modified;
}

bool ProjectState::isIdle() const
{
    return status == Status::Normal || status == Status::Errored;
}

bool ProjectState::canEdit() const
{
    return isUnpacked();
}

bool ProjectState::canSave() const
{
    return isUnpacked() && isIdle();
}

bool ProjectState::canInstall() const
{
    return isUnpacked() && isIdle();
}

bool ProjectState::canExplore() const
{
    return isUnpacked();
}

bool ProjectState::canClose() const
{
    return isIdle();
}
