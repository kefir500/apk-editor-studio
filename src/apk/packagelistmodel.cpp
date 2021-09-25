#include "apk/packagelistmodel.h"
#include "apk/package.h"
#include <QMessageBox>

PackageListModel::~PackageListModel()
{
    qDeleteAll(packages);
}

void PackageListModel::add(Package *package)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
        packages.append(package);
    endInsertRows();

    connect(package, &Package::stateUpdated, this, [=]() {
        const int row = packages.indexOf(package);
        emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
    });
}

bool PackageListModel::close(Package *package)
{
    const int row = packages.indexOf(package);
    if (row == -1) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
        packages.removeAt(row);
        package->deleteLater();
    endRemoveRows();

    return true;
}

Package *PackageListModel::at(int row) const
{
    return packages.at(row);
}

Package *PackageListModel::existing(const QString &filename) const
{
    QListIterator<Package *> it(packages);
    while (it.hasNext()) {
        Package *iteration = it.next();
        if (iteration->getOriginalPath() == QDir::toNativeSeparators(filename)) {
            return iteration;
        }
    }
    return nullptr;
}

QVariant PackageListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const int column = index.column();
        const auto package = static_cast<Package *>(index.internalPointer());
        if (role == Qt::DisplayRole) {
            switch (column) {
            case TitleColumn:
                return package->getTitle();
            case OriginalPathColumn:
                return package->getOriginalPath();
            case ContentsPathColumn:
                return package->getContentsPath();
            case IsUnpackedColumn:
                return package->getState().isUnpacked();
            case IsModifiedColumn:
                return package->getState().isModified();
            }
        } else if (role == Qt::DecorationRole) {
            switch (column) {
            case TitleColumn:
                return package->getThumbnail();
            case StatusColumn:
                switch (package->getState().getCurrentStatus()) {
                case PackageState::Status::Normal:
                    return QIcon::fromTheme("apk-idle");
                case PackageState::Status::Unpacking:
                    return QIcon::fromTheme("apk-opening");
                case PackageState::Status::Packing:
                case PackageState::Status::Signing:
                case PackageState::Status::Optimizing:
                    return QIcon::fromTheme("apk-saving");
                case PackageState::Status::Installing:
                    return QIcon::fromTheme("apk-installing");
                case PackageState::Status::Errored:
                    return QIcon::fromTheme("apk-error");
                }
            }
        }
    }
    return QVariant();
}

QModelIndex PackageListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row >= 0 && row < packages.count()) {
        return createIndex(row, column, packages.at(row));
    }
    return QModelIndex();
}

int PackageListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return packages.count();
}
