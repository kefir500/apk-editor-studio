#include "apk/iconsproxy.h"
#include "base/application.h"

IconsProxy::IconsProxy(QObject *parent) : QAbstractProxyModel(parent)
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

void IconsProxy::setSourceModel(ResourcesModel *sourceModel)
{
    if (this->sourceModel()) {
        disconnect(this->sourceModel(), &ResourcesModel::dataChanged, this, &IconsProxy::sourceDataChanged);
    }
    QAbstractProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, &ResourcesModel::dataChanged, this, &IconsProxy::sourceDataChanged);
}

bool IconsProxy::addIcon(const QPersistentModelIndex &index)
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

QString IconsProxy::getIconPath(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    return this->index(index.row(), IconsProxy::IconPath).data().toString();
}

QVariant IconsProxy::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const QModelIndex sourceIndex = mapToSource(index);
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case IconCaption:
                return sourceIndex.sibling(sourceIndex.row(), ResourcesModel::ResourceQualifiers).data().toString().toUpper();
            case IconPath:
                return sourceIndex.sibling(sourceIndex.row(), ResourcesModel::ResourcePath).data().toString();
            }
        } else if (role == Qt::ToolTipRole) {
            return sourceIndex.sibling(sourceIndex.row(), ResourcesModel::ResourcePath).data().toString();
        } else if (role == Qt::DecorationRole) {
            const QPixmap pixmap = sourceIndex.sibling(sourceIndex.row(), ResourcesModel::NodeCaption).data(Qt::DecorationRole).value<QPixmap>();
            return !pixmap.isNull() ? pixmap.scaled(app->scale(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation) : QPixmap();
        }
    }
    return QVariant();
}

QModelIndex IconsProxy::index(int row, int column, const QModelIndex &parent) const
{
    if (Q_UNLIKELY(parent.isValid())) {
        qWarning() << "CRITICAL: Unwanted parent passed to icons proxy";
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex IconsProxy::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

QModelIndex IconsProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (sourceIndex.isValid() && sourceToProxyMap.contains(sourceIndex)) {
        return createIndex(sourceToProxyMap[sourceIndex], sourceIndex.column(), sourceIndex.internalPointer());
    }
    return QModelIndex();
}

QModelIndex IconsProxy::mapToSource(const QModelIndex &proxyIndex) const
{
    if (proxyIndex.isValid() && proxyToSourceMap.contains(proxyIndex.row())) {
        QModelIndex sourceIndex = proxyToSourceMap.value(proxyIndex.row());
        return sourceIndex;
    }
    return QModelIndex();
}

int IconsProxy::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return proxyToSourceMap.count();
}

int IconsProxy::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

void IconsProxy::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}
