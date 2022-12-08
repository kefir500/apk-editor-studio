#include "base/iupdateinfo.h"

void IUpdateInfo::setMetadata(const QByteArray &metadata)
{
    this->metadata = metadata;
    emit updated();
}

bool IUpdateInfo::hasUpdates() const
{
    if (getLatestVersion().isEmpty()) {
        return false;
    }

    return isNewerVersion(getCurrentVersion(), getLatestVersion());
}

bool IUpdateInfo::isNewerVersion(const QString &currentVersion, const QString &latestVersion) const
{
    QStringList currentVersionSegments = currentVersion.split('.');
    QStringList latestVersionSegments = latestVersion.split('.');
    const int currentVersionSegmentCount = currentVersionSegments.size();
    const int latestVersionSegmentCount = latestVersionSegments.size();

    // Normalize segments count:
    const int minSegments = qMin(currentVersionSegmentCount, latestVersionSegmentCount);
    const int maxSegments = qMax(currentVersionSegmentCount, latestVersionSegmentCount);
    for (int i = minSegments; i < maxSegments; ++i) {
        if (currentVersionSegments.size() <= i) {
            currentVersionSegments.append("0");
        } else if (latestVersionSegments.size() <= i) {
            latestVersionSegments.append("0");
        }
    }

    // Compare segments:
    for (short i = 0; i < maxSegments; ++i) {
        const int currentVersionSegment = currentVersionSegments.at(i).toInt();
        const int latestVersionSegment = latestVersionSegments.at(i).toInt();
        if (currentVersionSegment != latestVersionSegment) {
            return latestVersionSegment > currentVersionSegment;
        }
    }
    return false;
}
