#include "apk/manifestmodel.h"
#include "base/utils.h"

ManifestModel::ManifestModel(QObject *parent) : QAbstractListModel(parent)
{
    manifest = nullptr;
    userdata.resize(5);
}

void ManifestModel::initialize(Manifest *manifest)
{
    this->manifest = manifest;
    emit dataChanged(index(0, 0), index(RowCount - 1, 0));
}

QString ManifestModel::getApplicationLabel() const
{
    return manifest->scopes.first()->label().getValue();
}

int ManifestModel::getVersionCode() const
{
    return manifest->getVersionCode();
}

QString ManifestModel::getVersionName() const
{
    return manifest->getVersionName();
}

int ManifestModel::getMinimumSdk() const
{
    return manifest->getMinSdk();
}

int ManifestModel::getTargetSdk() const
{
    return manifest->getTargetSdk();
}

bool ManifestModel::setApplicationLabel(const QString &label)
{
    if (manifest->scopes.first()->label().getValue() == label) {
        return false;
    }
    manifest->setApplicationLabel(label);
    emit dataChanged(index(ApplicationLabelRow, 0), index(ApplicationLabelRow, 0), {});
    return true;
}

bool ManifestModel::setVersionCode(int version)
{
    if (manifest->getVersionCode() == version) {
        return false;
    }
    manifest->setVersionCode(version);
    emit dataChanged(index(VersionCodeRow, 0), index(VersionCodeRow, 0), {});
    return true;
}

bool ManifestModel::setVersionName(const QString &version)
{
    if (manifest->getVersionName() == version) {
        return false;
    }
    manifest->setVersionName(version);
    emit dataChanged(index(VersionNameRow, 0), index(VersionNameRow, 0), {});
    return true;
}

bool ManifestModel::setMinimumSdk(int sdk)
{
    if (manifest->getMinSdk() == sdk) {
        return false;
    }
    manifest->setMinSdk(sdk);
    emit dataChanged(index(MinimumSdkRow, 0), index(MinimumSdkRow, 0), {});
    return true;
}

bool ManifestModel::setTargetSdk(int sdk)
{
    if (manifest->getTargetSdk() == sdk) {
        return false;
    }
    manifest->setTargetSdk(sdk);
    emit dataChanged(index(TargetSdkRow, 0), index(TargetSdkRow, 0), {});
    return true;
}

bool ManifestModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (manifest && index.isValid()) {
        if (role == Qt::EditRole) {
            const int row = index.row();
            switch (row) {
            case ApplicationLabelRow:
                return setApplicationLabel(value.toString());
            case VersionCodeRow:
                return setVersionCode(value.toInt());
            case VersionNameRow:
                return setVersionName(value.toString());
            case MinimumSdkRow:
                return setMinimumSdk(value.toInt());
            case TargetSdkRow:
                return setTargetSdk(value.toInt());
            }
        } else if (role >= Qt::UserRole) {
            userdata[index.row()][role] = value;
            emit dataChanged(index, index, {role});
        }
    }
    return false;
}

QVariant ManifestModel::data(const QModelIndex &index, int role) const
{
    auto getSdkTitle = [](int api) -> QString {
        const QString codename = Utils::getAndroidCodename(api);
        return !codename.isEmpty() ? QString("%1 (%2)").arg(api).arg(codename) : QString::number(api);
    };

    if (manifest && index.isValid()) {
        const int row = index.row();
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (row) {
            case ApplicationLabelRow:
                return getApplicationLabel();
            case VersionCodeRow:
                return getVersionCode();
            case VersionNameRow:
                return getVersionName();
            case MinimumSdkRow: {
                const int sdk = getMinimumSdk();
                if (role == Qt::DisplayRole) {
                    return getSdkTitle(sdk);
                } else {
                    return sdk;
                }
            }
            case TargetSdkRow:
                const int sdk = getTargetSdk();
                if (role == Qt::DisplayRole) {
                    return getSdkTitle(sdk);
                } else {
                    return sdk;
                }
            }
        } else if (role == ReferenceRole) {
            switch (row) {
            case ApplicationLabelRow:
                return getApplicationLabel().startsWith("@string/");
            case MinimumSdkRow:
            case TargetSdkRow:
                return true;
            }
        } else if (role >= Qt::UserRole) {
            return userdata.at(index.row())[role];
        }
    }
    return QVariant();
}

QVariant ManifestModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case ApplicationLabelRow:
                return tr("Application Title");
            case VersionCodeRow:
                return tr("Version Code");
            case VersionNameRow:
                return tr("Version Name");
            case MinimumSdkRow:
                return tr("Minimum SDK");
            case TargetSdkRow:
                return tr("Target SDK");
            }
        }
    }
    return QVariant();
}

QModelIndex ManifestModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

int ManifestModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return RowCount;
}

Qt::ItemFlags ManifestModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
