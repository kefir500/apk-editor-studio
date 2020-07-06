#include "base/androidfilesystemmodel.h"
#include "base/utils.h"
#include "tools/adb.h"
#include <QDir>
#include <QRegularExpression>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

AndroidFileSystemModel::AndroidFileSystemModel(const QString &serial, QObject *parent)
    : QAbstractTableModel(parent)
    , serial(serial)
{
    emit pathChanged("/");
    ls();
}

bool AndroidFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)
    if (index.isValid() && index.column() == NameColumn) {
        if (value != getItemName(index)) {
            const QString fullPath = getItemPath(index);
            const QString directory = QFileInfo(fullPath).path();
            rename(fullPath, QString("%1/%2").arg(directory, value.toString()));
            return true;
        }
    }
    return false;
}

QVariant AndroidFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch (index.column()) {
            case NameColumn:
                return getItemName(index);
            case PathColumn:
                return getItemPath(index);
            }
        case Qt::DecorationRole:
            return getItemIcon(index);
        case FileTypeRole:
            return getItemType(index);
        }
    }
    return {};
}

QModelIndex AndroidFileSystemModel::index(int row, int column, const QModelIndex &) const
{
    if (row >= 0 && row < rowCount()) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

int AndroidFileSystemModel::rowCount(const QModelIndex &) const
{
    return fileSystemItems.count();
}

int AndroidFileSystemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

Qt::ItemFlags AndroidFileSystemModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QString AndroidFileSystemModel::getItemName(const QModelIndex &index) const
{
    return fileSystemItems.at(index.row()).getName();
}

QString AndroidFileSystemModel::getItemPath(const QModelIndex &index) const
{
    return fileSystemItems.at(index.row()).getPath();
}

QIcon AndroidFileSystemModel::getItemIcon(const QModelIndex &index) const
{
    return fileSystemItems.at(index.row()).getIcon();
}

AndroidFileSystemItem::Type AndroidFileSystemModel::getItemType(const QModelIndex &index) const
{
    return fileSystemItems.at(index.row()).getType();
}

const QString &AndroidFileSystemModel::getCurrentPath() const
{
    return currentPath;
}

void AndroidFileSystemModel::cd(const QString &path)
{
    beginResetModel();
    QString directory;
    if (path.startsWith('/')) {
        directory = path;
    } else if (currentPath == "/") {
        directory = QString("/%1").arg(path);
    } else {
        directory = QString("%1/%2").arg(currentPath, path);
    }
    directory = QDir::cleanPath(Utils::normalizePath(directory));
    auto shell = new Adb::Cd(path, serial, this);
    connect(shell, &Adb::Cd::finished, this, [=](bool success) {
        if (success) {
            currentPath = directory;
            emit pathChanged(directory);
            ls();
        } else {
            endResetModel();
            emit error(tr("Could not open the directory."));
        }
        shell->deleteLater();
    });
    shell->run();
}

void AndroidFileSystemModel::copy(const QString &src, const QString &dst)
{
    auto adb = new Adb::Cp(src, dst + '/', serial, this);
    connect(adb, &Adb::Cp::finished, this, [=](bool success) {
        if (success) {
            if (dst == currentPath) {
                ls();
            }
        } else {
            emit error(tr("Could not copy the file or directory."));
        }
        adb->deleteLater();
    });
    adb->run();
}

void AndroidFileSystemModel::move(const QString &src, const QString &dst)
{
    auto adb = new Adb::Mv(src, dst + '/', serial, this);
    connect(adb, &Adb::Mv::finished, this, [=](bool success) {
        if (success) {
            if (dst == currentPath) {
                ls();
            }
        } else {
            emit error(tr("Could not move the file or directory."));
        }
        adb->deleteLater();
    });
    adb->run();
}

void AndroidFileSystemModel::rename(const QString &src, const QString &dst)
{
    auto adb = new Adb::Mv(src, dst, serial, this);
    connect(adb, &Adb::Mv::finished, this, [=](bool success) {
        if (success) {
            ls();
        } else {
            emit error(tr("Could not rename the file or directory."));
        }
        adb->deleteLater();
    });
    adb->run();
}

void AndroidFileSystemModel::remove(const QString &path)
{
    auto adb = new Adb::Rm(path, serial, this);
    connect(adb, &Adb::Rm::finished, this, [=](bool success) {
        if (success) {
            ls();
        } else {
            emit error(tr("Could not delete the file or directory."));
        }
        adb->deleteLater();
    });
    adb->run();
}

void AndroidFileSystemModel::download(const QString &src, const QString &dst)
{
    auto adb = new Adb::Pull(src, dst, serial, this);
    connect(adb, &Adb::Pull::finished, this, [=](bool success) {
        if (!success) {
            emit error(tr("Could not download the file or directory."));
        }
        adb->deleteLater();
    });
    adb->run();
}

void AndroidFileSystemModel::upload(const QString &src, const QString &dst)
{
    auto adb = new Adb::Push(src, dst, serial, this);
    connect(adb, &Adb::Push::finished, this, [=](bool success) {
        if (success) {
            if (dst == currentPath) {
                ls();
            }
        } else {
            emit error(tr("Could not upload the file."));
        }
        adb->deleteLater();
    });
    adb->run();
}

void AndroidFileSystemModel::ls()
{
    beginResetModel();
    auto shell = new Adb::Ls(currentPath, serial, this);
    connect(shell, &Adb::Ls::finished, this, [=](bool success) {
        Q_UNUSED(success)
        fileSystemItems.clear();
        fileSystemItems.append(shell->getFileSystemItems());
        endResetModel();
        shell->deleteLater();
    });
    shell->run();
}
