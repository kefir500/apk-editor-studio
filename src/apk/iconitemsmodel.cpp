#include "apk/iconitemsmodel.h"
#include "base/application.h"
#include <QDebug>

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

void IconItemsModel::setSourceModel(ResourceItemsModel *sourceModel)
{
    if (this->sourceModel()) {
        disconnect(this->sourceModel(), &ResourceItemsModel::dataChanged, this, &IconItemsModel::sourceDataChanged);
    }
    QAbstractProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, &ResourceItemsModel::dataChanged, this, &IconItemsModel::sourceDataChanged);
}

bool IconItemsModel::addIcon(const QPersistentModelIndex &index)
{
    if (!sourceToProxyMap.contains(index)) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
            const int row = proxyToSourceMap.size();
            sourceToProxyMap[index] = row;
            proxyToSourceMap[row] = index;
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
        const QPixmap pixmap = sourceIndex.data(Qt::DecorationRole).value<QPixmap>();
        icon.addPixmap(pixmap);
    }
    return icon;
}

QString IconItemsModel::getIconPath(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    return this->index(index.row(), IconItemsModel::IconPath).data().toString();
}

ResourceItemsModel *IconItemsModel::sourceModel() const
{
    return static_cast<ResourceItemsModel *>(QAbstractProxyModel::sourceModel());
}

QVariant IconItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const QModelIndex sourceIndex = mapToSource(index);
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case IconCaption:
                return sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourceQualifiers).data().toString().toUpper();
            case IconPath:
                return sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourcePath).data().toString();
            }
        } else if (role == Qt::ToolTipRole) {
            return sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourcePath).data().toString();
        } else if (role == Qt::DecorationRole) {
            const QPixmap pixmap = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::NodeCaption).data(Qt::DecorationRole).value<QPixmap>();
            return !pixmap.isNull() ? pixmap.scaled(app->scale(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation) : QPixmap();
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
    if (sourceIndex.isValid() && sourceToProxyMap.contains(sourceIndex)) {
        return createIndex(sourceToProxyMap[sourceIndex], sourceIndex.column(), sourceIndex.internalPointer());
    }
    return QModelIndex();
}

QModelIndex IconItemsModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (proxyIndex.isValid() && proxyToSourceMap.contains(proxyIndex.row())) {
        QModelIndex sourceIndex = proxyToSourceMap.value(proxyIndex.row());
        return sourceIndex;
    }
    return QModelIndex();
}

int IconItemsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return proxyToSourceMap.count();
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
