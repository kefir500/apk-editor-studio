#include "apk/iconitemsmodel.h"
#include "base/application.h"
#include "base/utils.h"
#include <QFileIconProvider>
#include <QDebug>

IconItemsModel::~IconItemsModel()
{
    qDeleteAll(icons);
}

void IconItemsModel::setSourceModel(ResourceItemsModel *newSourceModel)
{
    beginResetModel();
        if (sourceModel()) {
            disconnect(sourceModel(), &ResourceItemsModel::added, this, &IconItemsModel::onResourceAdded);
            disconnect(sourceModel(), &ResourceItemsModel::removed, this, &IconItemsModel::onResourceRemoved);
            disconnect(sourceModel(), &ResourceItemsModel::dataChanged, this, &IconItemsModel::onResourceChanged);
        }
        QAbstractProxyModel::setSourceModel(newSourceModel);
        if (sourceModel()) {
            connect(sourceModel(), &ResourceItemsModel::added, this, &IconItemsModel::onResourceAdded);
            connect(sourceModel(), &ResourceItemsModel::removed, this, &IconItemsModel::onResourceRemoved);
            connect(sourceModel(), &ResourceItemsModel::dataChanged, this, &IconItemsModel::onResourceChanged);
        }
    endResetModel();
}

QIcon IconItemsModel::getIcon() const
{
    QIcon icon;
    for (const IconItem *iconItem : icons) {
        if (iconItem->type == Icon) {
            const QPixmap pixmap = iconItem->index.data(Qt::DecorationRole).value<QPixmap>();
            icon.addPixmap(pixmap);
        }
    }
    return icon;
}

QIcon IconItemsModel::getIcon(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (Q_LIKELY(sourceIndex.isValid())) {
        QIcon icon = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::NodeCaption).data(Qt::DecorationRole).value<QPixmap>();
        return !icon.isNull() ? icon : QFileIconProvider().icon(getIconPath(index));
    }
    return QIcon();
}

QString IconItemsModel::getIconPath(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (Q_LIKELY(sourceIndex.isValid())) {
        return sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourcePath).data().toString();
    }
    return QString();
}

QString IconItemsModel::getIconCaption(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    const IconItem *iconItem = icons.at(index.row());
    const QModelIndex sourceIndex = iconItem->index;
    QString caption = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourceQualifiers).data().toString().toUpper();
    switch (iconItem->type) {
    case Icon:
        break;
    case RoundIcon:
        caption.append(QString(" (%1)").arg(tr("Round icon")));
        break;
    case Banner:
        caption.append(QString(" (%1)").arg(tr("TV Banner")));
        break;
    }
    return caption;
}

ResourceItemsModel *IconItemsModel::sourceModel() const
{
    return static_cast<ResourceItemsModel *>(QAbstractProxyModel::sourceModel());
}

QVariant IconItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case IconCaption: return getIconCaption(index);
                case IconPath: return getIconPath(index);
            }
        } else if (role == Qt::ToolTipRole) {
            return getIconPath(index);
        } else if (role == Qt::DecorationRole) {
            return getIcon(index);
        }
    }
    return QVariant();
}

QModelIndex IconItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (Q_UNLIKELY(parent.isValid())) {
        qWarning() << "CRITICAL: Unwanted parent passed to icons proxy";
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex IconItemsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

QModelIndex IconItemsModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (sourceIndex.isValid()) {
        IconItem *iconItem = sourceToProxyMap.value(sourceIndex);
        if (iconItem) {
            const int row = icons.indexOf(iconItem);
            if (Q_LIKELY(row != -1)) {
                return createIndex(row, sourceIndex.column(), sourceIndex.internalPointer());
            }
        }
    }
    return QModelIndex();
}

QModelIndex IconItemsModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (proxyIndex.isValid()) {
        const int row = proxyIndex.row();
        if (Q_LIKELY(row >= 0 && row < icons.count())) {
            QModelIndex sourceIndex = icons.at(row)->index;
            return sourceIndex;
        }
    }
    return QModelIndex();
}

void IconItemsModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)
    std::sort(icons.begin(), icons.end(), [](const IconItem *icon1, const IconItem *icon2) {
        auto index1 = icon1->index;
        auto index2 = icon2->index;
        auto dpi1 = index1.sibling(index1.row(), ResourceItemsModel::ResourceDpi).data(ResourceItemsModel::SortRole);
        auto dpi2 = index2.sibling(index2.row(), ResourceItemsModel::ResourceDpi).data(ResourceItemsModel::SortRole);
        if (icon1->type < icon2->type) { return true; }
        if (icon2->type < icon1->type) { return false; }
        if (dpi1 < dpi2) { return true; }
        if (dpi2 < dpi1) { return false; }
        return false;
    });
}

int IconItemsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return icons.count();
}

int IconItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

bool IconItemsModel::addIcon(const QPersistentModelIndex &index, IconType type)
{
    if (!sourceToProxyMap.contains(index)) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
            IconItem *icon = new IconItem(index, type);
            icons.append(icon);
            sourceToProxyMap.insert(index, icon);
        endInsertRows();
        return true;
    }
    return false;
}

void IconItemsModel::onResourceAdded(const QModelIndex &index)
{
    auto resource = sourceModel()->getResource(index);
    if (resource && Utils::isDrawableResource(resource->getFilePath())) {
        auto resourceName = resource->getName();
        auto resourceType = resource->getType();
        if (!resourceName.isEmpty() && !resourceType.isEmpty()) {
            auto iconResource = apk()->getManifest()->getApplicationIcon();
            auto roundIconResource = apk()->getManifest()->getApplicationRoundIcon();
            auto bannerResource = apk()->getManifest()->getApplicationBanner();
            if (resourceName == iconResource.getResourceName() && resourceType == iconResource.getResourceType()) {
                addIcon(index, Icon);
            } else if (resourceName == roundIconResource.getResourceName() && resourceType == roundIconResource.getResourceType()) {
                addIcon(index, RoundIcon);
            } else if (resourceName == bannerResource.getResourceName() && resourceType == bannerResource.getResourceType()) {
                addIcon(index, Banner);
            }
        }
    }
}

void IconItemsModel::onResourceRemoved(const QModelIndex &index)
{
    // TODO
}

void IconItemsModel::onResourceChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

const Project *IconItemsModel::apk()
{
    return sourceModel()->getApk();
}
