#include "apk/iconitemsmodel.h"
#include "base/application.h"
#include <QDebug>

// IconItemsModel:

IconItemsModel::IconItemsModel(QObject *parent) : QAbstractProxyModel(parent)
{
    connect(this, &QAbstractItemModel::dataChanged, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        auto sourceModel = this->sourceModel();
        if (sourceModel) {
            blockSignals(true);
            emit sourceModel->dataChanged(mapToSource(topLeft), mapToSource(bottomRight));
            blockSignals(false);
        }
    });
}

IconItemsModel::~IconItemsModel()
{
    qDeleteAll(icons);
}

void IconItemsModel::setSourceModel(ResourceItemsModel *sourceModel)
{
    if (this->sourceModel()) {
        disconnect(this->sourceModel(), &ResourceItemsModel::dataChanged, this, &IconItemsModel::sourceDataChanged);
    }
    QAbstractProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, &ResourceItemsModel::dataChanged, this, &IconItemsModel::sourceDataChanged);
}

bool IconItemsModel::addIcon(const QPersistentModelIndex &index, IconType type)
{
    if (!sourceToProxyMap.contains(index)) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
            IconItem *icon = new IconItem(index);
            icon->setType(type);
            icons.append(icon);
            std::sort(icons.begin(), icons.end(), [](const IconItem *a, const IconItem *b) {
                return a->getType() < b->getType();
            });
            sourceToProxyMap.insert(index, icon);
        endInsertRows();
        return true;
    }
    return false;
}

QIcon IconItemsModel::getIcon() const
{
    QIcon icon;
    const int rows = rowCount();
    for (int i = 0; i < rows; ++i) {
        const QModelIndex sourceIndex = mapToSource(index(i, ResourceItemsModel::NodeCaption));
        if (Q_LIKELY(sourceIndex.isValid())) {
            const QPixmap pixmap = sourceIndex.data(Qt::DecorationRole).value<QPixmap>();
            icon.addPixmap(pixmap);
        }
    }
    return icon;
}

QPixmap IconItemsModel::getPixmap(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (Q_LIKELY(sourceIndex.isValid())) {
        // TODO Cut to square
        const QPixmap pixmap = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::NodeCaption).data(Qt::DecorationRole).value<QPixmap>();
        return !pixmap.isNull() ? pixmap.scaled(app->scale(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation) : QPixmap();
    }
    return QPixmap();
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
    const QModelIndex sourceIndex = iconItem->getIndex();
    QString caption = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourceQualifiers).data().toString().toUpper();
    switch (iconItem->getType()) {
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
            return getPixmap(index);
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
            QModelIndex sourceIndex = icons.at(row)->getIndex();
            return sourceIndex;
        }
    }
    return QModelIndex();
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

void IconItemsModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

// IconItem:

const QPersistentModelIndex &IconItemsModel::IconItem::getIndex() const
{
    return index;
}

IconItemsModel::IconType IconItemsModel::IconItem::getType() const
{
    return type;
}

void IconItemsModel::IconItem::setType(IconType type)
{
    this->type = type;
}
