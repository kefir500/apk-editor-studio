#include "apk/packagestate.h"

PackageState::PackageState()
{
    unpacked = false;
    modified = false;
    status = Status::Normal;
}

void PackageState::setCurrentStatus(const Status &status)
{
    this->status = status;
    emit changed();
}

void PackageState::setUnpacked(bool unpacked)
{
    this->unpacked = unpacked;
    emit changed();
}

void PackageState::setModified(bool modified)
{
    this->modified = modified;
    emit changed();
}

const PackageState::Status &PackageState::getCurrentStatus() const
{
    return status;
}

bool PackageState::isUnpacked() const
{
    return unpacked;
}

bool PackageState::isModified() const
{
    return modified;
}

bool PackageState::isIdle() const
{
    return status == Status::Normal || status == Status::Errored;
}

bool PackageState::canEdit() const
{
    return isUnpacked();
}

bool PackageState::canSave() const
{
    return isUnpacked() && isIdle();
}

bool PackageState::canInstall() const
{
    return isUnpacked() && isIdle();
}

bool PackageState::canExplore() const
{
    return isUnpacked();
}

bool PackageState::canClose() const
{
    return isIdle();
}
