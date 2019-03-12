#include "apk/resourcemodelindex.h"
#include "apk/resourceitemsmodel.h"
#include "apk/iconitemsmodel.h"
#include <QFileSystemModel>
#include <QFileIconProvider>

QString ResourceModelIndex::path() const
{
    if (isValid()) {
        auto resourcesModel = qobject_cast<const ResourceItemsModel *>(model());
        if (resourcesModel) {
            return resourcesModel->getResource(*this)->getFilePath();
        }
        auto iconsModel = qobject_cast<const IconItemsModel *>(model());
        if (iconsModel) {
            return iconsModel->getIconPath(*this);
        }
        auto fileSystemModel = qobject_cast<const QFileSystemModel *>(model());
        if (fileSystemModel) {
            return fileSystemModel->filePath(*this);
        }
    }
    return QString();
}

QIcon ResourceModelIndex::icon() const
{
    auto fetch = [this]() -> QIcon {
        auto resourcesModel = qobject_cast<const ResourceItemsModel *>(model());
        if (resourcesModel) {
            return resourcesModel->getResource(*this)->getLanguageIcon();
        }
        auto iconsModel = qobject_cast<const IconItemsModel *>(model());
        if (iconsModel) {
            auto resourcesModel = iconsModel->sourceModel();
            return resourcesModel->getResource(iconsModel->mapToSource(*this))->getLanguageIcon();
        }
        auto fileSystemModel = qobject_cast<const QFileSystemModel *>(model());
        if (fileSystemModel) {
            return fileSystemModel->fileIcon(this->sibling(this->row(), 0));
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
