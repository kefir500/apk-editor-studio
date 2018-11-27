#include "apk/projectstate.h"

ProjectState::ProjectState()
{
    unpacked = false;
    modified = false;
    lastActionFailed = false;
    currentAction = ProjectIdle;
}

void ProjectState::setCurrentAction(ProjectAction action)
{
    currentAction = action;
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

void ProjectState::setLastActionFailed(bool fail)
{
    lastActionFailed = fail;
    emit changed();
}

ProjectState::ProjectAction ProjectState::getCurrentAction() const
{
    return currentAction;
}

bool ProjectState::isUnpacked() const
{
    return unpacked;
}

bool ProjectState::isModified() const
{
    return modified;
}

bool ProjectState::isLastActionFailed() const
{
    return lastActionFailed;
}

bool ProjectState::canEdit() const
{
    return isUnpacked();
}

bool ProjectState::canSave() const
{
    return isUnpacked() && (currentAction == ProjectIdle);
}

bool ProjectState::canInstall() const
{
    return isUnpacked() && (currentAction == ProjectIdle);
}

bool ProjectState::canExplore() const
{
    return isUnpacked();
}

bool ProjectState::canClose() const
{
    return isUnpacked() && (currentAction == ProjectIdle);
}
