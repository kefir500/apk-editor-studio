#include "apk/projectstate.h"

ProjectState::ProjectState()
{
    unpacked = false;
    modified = false;
    state = StateNormal;
}

void ProjectState::setCurrentState(const State &state)
{
    this->state = state;
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

const ProjectState::State &ProjectState::getCurrentState() const
{
    return state;
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
    return state == StateNormal || state == StateErrored;
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
