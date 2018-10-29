#include "apk/manifestmodel.h"
#include "base/debug.h"

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

bool ManifestModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (manifest && index.isValid()) {
        if (role == Qt::EditRole) {
            const int row = index.row();
            switch (row) {
                case ApplicationLabel: manifest->setApplicationLabel(value.toString()); break;
                case VersionCode:      manifest->setVersionCode(value.toInt()); break;
                case VersionName:      manifest->setVersionName(value.toString()); break;
                case MinimumSdk:       manifest->setMinSdk(value.toInt()); break;
                case TargetSdk:        manifest->setTargetSdk(value.toInt()); break;
            }
        } else if (role >= Qt::UserRole) {
            userdata[index.row()][role] = value;
        } else {
            return false;
        }
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

QVariant ManifestModel::data(const QModelIndex &index, int role) const
{
    if (manifest && index.isValid()) {
        const int row = index.row();
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (row) {
                case ApplicationLabel: return manifest->getApplicationLabel();
                case VersionCode:      return manifest->getVersionCode();
                case VersionName:      return manifest->getVersionName();
                case MinimumSdk:       return manifest->getMinSdk();
                case TargetSdk:        return manifest->getTargetSdk();
            }
        } else if (role == ReferenceRole && row == ApplicationLabel) {
            return manifest->getApplicationLabel().startsWith("@string/");
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
                case ApplicationLabel: return tr("Application Title");
                case VersionCode:      return tr("Version Code");
                case VersionName:      return tr("Version Name");
                case MinimumSdk:       return tr("Minimum SDK");
                case TargetSdk:        return tr("Target SDK");
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
    Q_UNUSED(parent);
    return RowCount;
}

Qt::ItemFlags ManifestModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
