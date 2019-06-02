#include "apk/resourcemodelindex.h"
#include "apk/resourceitemsmodel.h"
#include "apk/iconitemsmodel.h"
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QSortFilterProxyModel>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

QString ResourceModelIndex::path() const
{
    if (isValid()) {
        auto mapping = map();
        QAbstractItemModel *model = mapping.first;
        QModelIndex index = mapping.second;

        auto resourcesModel = qobject_cast<const ResourceItemsModel *>(model);
        if (resourcesModel) {
            auto resource = resourcesModel->getResource(index);
            return resource ? resource->getFilePath() : QString();
        }
        auto iconsModel = qobject_cast<const IconItemsModel *>(model);
        if (iconsModel) {
            return iconsModel->getIconPath(index);
        }
        auto fileSystemModel = qobject_cast<const QFileSystemModel *>(model);
        if (fileSystemModel) {
            return fileSystemModel->filePath(index);
        }
    }
    return QString();
}

QIcon ResourceModelIndex::icon() const
{
    auto fetch = [this]() -> QIcon {
        auto mapping = map();
        QAbstractItemModel *model = mapping.first;
        QModelIndex index = mapping.second;

        auto resourcesModel = qobject_cast<const ResourceItemsModel *>(model);
        if (resourcesModel) {
            return resourcesModel->getResource(index)->getLanguageIcon();
        }
        auto iconsModel = qobject_cast<const IconItemsModel *>(model);
        if (iconsModel) {
            auto resourcesModel = iconsModel->sourceModel();
            return resourcesModel->getResource(iconsModel->mapToSource(index))->getLanguageIcon();
        }
        auto fileSystemModel = qobject_cast<const QFileSystemModel *>(model);
        if (fileSystemModel) {
            return fileSystemModel->fileIcon(index.sibling(index.row(), 0));
        }
        return QIcon();
    };

    if (isValid()) {
        QIcon icon = fetch();
        return !icon.isNull() ? icon : QFileIconProvider().icon(QFileInfo(path()));
    }
    return QIcon();
}

void ResourceModelIndex::update()
{
    emit const_cast<QAbstractItemModel *>(model())->dataChanged(*this, *this);
}

QPair<QAbstractItemModel *, QModelIndex> ResourceModelIndex::map() const
{
    QAbstractItemModel *model = nullptr;
    QModelIndex index = {};

    auto sortProxy = qobject_cast<const QSortFilterProxyModel *>(this->model());
    if (!sortProxy) {
        model = const_cast<QAbstractItemModel *>(this->model());
        index = *this;
    } else {
        model = sortProxy->sourceModel();
        index = sortProxy->mapToSource(*this);
    }

    return {model, index};
}
