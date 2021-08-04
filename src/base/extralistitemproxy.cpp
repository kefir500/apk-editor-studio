#include "base/extralistitemproxy.h"

void ExtraListItemProxy::appendRow()
{
    const int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    appendedRows.append(QMap<int, QVariant>());
    endInsertRows();
}

void ExtraListItemProxy::appendRows(int count)
{
    const int row = rowCount();
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        appendedRows.append(QMap<int, QVariant>());
    }
    endInsertRows();
}

void ExtraListItemProxy::prependRow()
{
    beginInsertRows(QModelIndex(), 0, 0);
    prependedRows.prepend(QMap<int, QVariant>());
    endInsertRows();
}

void ExtraListItemProxy::prependRows(int count)
{
    beginInsertRows(QModelIndex(), 0, rowCount() + count - 1);
    for (int i = 0; i < count; ++i) {
        prependedRows.prepend(QMap<int, QVariant>());
    }
    endInsertRows();
}

bool ExtraListItemProxy::isRowAppended(int row) const
{
    return row > prependedRows.size() + sourceModel()->rowCount();
}

bool ExtraListItemProxy::isRowPrepended(int row) const
{
    return row < prependedRows.size();
}

int ExtraListItemProxy::mapFromSourceRow(int row) const
{
    return row + prependedRows.size();
}

int ExtraListItemProxy::mapToSourceRow(int row) const
{
    return row - prependedRows.size();
}

void ExtraListItemProxy::setSourceModel(QAbstractItemModel *newSourceModel)
{
    beginResetModel();
    auto oldSourceModel = sourceModel();
    if (oldSourceModel) {
        disconnect(oldSourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ExtraListItemProxy::sourceRowsAboutToBeInserted);
        disconnect(oldSourceModel, &QAbstractItemModel::rowsInserted, this, &ExtraListItemProxy::sourceRowsInserted);
        disconnect(oldSourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ExtraListItemProxy::sourceRowsAboutToBeRemoved);
        disconnect(oldSourceModel, &QAbstractItemModel::rowsRemoved, this, &ExtraListItemProxy::sourceRowsRemoved);
        disconnect(oldSourceModel, &QAbstractItemModel::rowsAboutToBeMoved, this, &ExtraListItemProxy::sourceRowsAboutToBeMoved);
        disconnect(oldSourceModel, &QAbstractItemModel::rowsMoved, this, &ExtraListItemProxy::sourceRowsMoved);
        disconnect(oldSourceModel, &QAbstractItemModel::columnsAboutToBeInserted, this, &ExtraListItemProxy::sourceColumnsAboutToBeInserted);
        disconnect(oldSourceModel, &QAbstractItemModel::columnsInserted, this, &ExtraListItemProxy::sourceColumnsInserted);
        disconnect(oldSourceModel, &QAbstractItemModel::columnsAboutToBeRemoved, this, &ExtraListItemProxy::sourceColumnsAboutToBeRemoved);
        disconnect(oldSourceModel, &QAbstractItemModel::columnsRemoved, this, &ExtraListItemProxy::sourceColumnsRemoved);
        disconnect(oldSourceModel, &QAbstractItemModel::columnsAboutToBeMoved, this, &ExtraListItemProxy::sourceColumnsAboutToBeMoved);
        disconnect(oldSourceModel, &QAbstractItemModel::columnsMoved, this, &ExtraListItemProxy::sourceColumnsMoved);
        disconnect(oldSourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &ExtraListItemProxy::sourceModelAboutToBeReset);
        disconnect(oldSourceModel, &QAbstractItemModel::modelReset, this, &ExtraListItemProxy::sourceModelReset);
        disconnect(oldSourceModel, &QAbstractItemModel::dataChanged, this, &ExtraListItemProxy::sourceDataChanged);
        disconnect(oldSourceModel, &QAbstractItemModel::headerDataChanged, this, &ExtraListItemProxy::sourceHeaderDataChanged);
        // TODO Handle layoutAboutToBeChanged
        // TODO Handle layoutChanged
    }
    QAbstractProxyModel::setSourceModel(newSourceModel);
    if (newSourceModel) {
        connect(newSourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ExtraListItemProxy::sourceRowsAboutToBeInserted);
        connect(newSourceModel, &QAbstractItemModel::rowsInserted, this, &ExtraListItemProxy::sourceRowsInserted);
        connect(newSourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ExtraListItemProxy::sourceRowsAboutToBeRemoved);
        connect(newSourceModel, &QAbstractItemModel::rowsRemoved, this, &ExtraListItemProxy::sourceRowsRemoved);
        connect(newSourceModel, &QAbstractItemModel::rowsAboutToBeMoved, this, &ExtraListItemProxy::sourceRowsAboutToBeMoved);
        connect(newSourceModel, &QAbstractItemModel::rowsMoved, this, &ExtraListItemProxy::sourceRowsMoved);
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeInserted, this, &ExtraListItemProxy::sourceColumnsAboutToBeInserted);
        connect(newSourceModel, &QAbstractItemModel::columnsInserted, this, &ExtraListItemProxy::sourceColumnsInserted);
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeRemoved, this, &ExtraListItemProxy::sourceColumnsAboutToBeRemoved);
        connect(newSourceModel, &QAbstractItemModel::columnsRemoved, this, &ExtraListItemProxy::sourceColumnsRemoved);
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeMoved, this, &ExtraListItemProxy::sourceColumnsAboutToBeMoved);
        connect(newSourceModel, &QAbstractItemModel::columnsMoved, this, &ExtraListItemProxy::sourceColumnsMoved);
        connect(newSourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &ExtraListItemProxy::sourceModelAboutToBeReset);
        connect(newSourceModel, &QAbstractItemModel::modelReset, this, &ExtraListItemProxy::sourceModelReset);
        connect(newSourceModel, &QAbstractItemModel::dataChanged, this, &ExtraListItemProxy::sourceDataChanged);
        connect(newSourceModel, &QAbstractItemModel::headerDataChanged, this, &ExtraListItemProxy::sourceHeaderDataChanged);
        // TODO Handle layoutAboutToBeChanged
        // TODO Handle layoutChanged
    };
    endResetModel();
}

bool ExtraListItemProxy::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    const int row = index.row();
    if (isRowPrepended(row)) {
        prependedRows[row][role] = value;
        return true;
    }
    if (isRowAppended(row)) {
        appendedRows[row][role] = value;
        return true;
    }
    return sourceModel()->setData(mapToSource(index), value, role);
}

QVariant ExtraListItemProxy::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid()) {
        return QVariant();
    }
    const int row = proxyIndex.row();
    const int column = proxyIndex.column();
    if (isRowPrepended(row)) {
        return column == 0 ? prependedRows.at(row).value(role) : QVariant();
    }
    if (isRowAppended(row)) {
        return column == 0 ? appendedRows.at(row - sourceModel()->rowCount()).value(role) : QVariant();
    }
    return mapToSource(proxyIndex).data(role);
}

QModelIndex ExtraListItemProxy::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    if (isRowPrepended(row) || isRowAppended(row)) {
        return createIndex(row, column);
    }
    if (sourceModel()) {
        auto ptr = sourceModel()->index(mapToSourceRow(row), column).internalPointer();
        return createIndex(row, column, ptr);
    }
    return QModelIndex();
}

QModelIndex ExtraListItemProxy::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

QModelIndex ExtraListItemProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
    const int proxyRow = sourceIndex.row() + prependedRows.size();
    const int proxyColumn = sourceIndex.column();
    return createIndex(proxyRow, proxyColumn, sourceIndex.internalPointer());
}

QModelIndex ExtraListItemProxy::mapToSource(const QModelIndex &proxyIndex) const
{
    const int row = proxyIndex.row();
    if (isRowPrepended(row) || isRowAppended(row)) {
        return QModelIndex();
    }
    return sourceModel()->index(row - prependedRows.size(), proxyIndex.column(), proxyIndex.parent());
}

int ExtraListItemProxy::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    const int extraRowsCount = prependedRows.size() + appendedRows.size();
    if (sourceModel()) {
        return extraRowsCount + sourceModel()->rowCount();
    } else {
        return extraRowsCount;
    }
}

int ExtraListItemProxy::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    return sourceModel() ? sourceModel()->columnCount() : 1;
}

Qt::ItemFlags ExtraListItemProxy::flags(const QModelIndex &index) const
{
    const int row = index.row();
    if (isRowPrepended(row) || isRowAppended(row)) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return mapToSource(index).flags();
}

void ExtraListItemProxy::sourceRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    beginInsertRows(QModelIndex(), mapFromSourceRow(first), mapFromSourceRow(last));
}

void ExtraListItemProxy::sourceRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(first)
    Q_UNUSED(last)
    endInsertRows();
}

void ExtraListItemProxy::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    beginRemoveRows(QModelIndex(), mapFromSourceRow(first), mapFromSourceRow(last));
}

void ExtraListItemProxy::sourceRowsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(first)
    Q_UNUSED(last)
    endRemoveRows();
}

void ExtraListItemProxy::sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_ASSERT(!sourceParent.isValid());
    Q_ASSERT(!destinationParent.isValid());
    beginMoveRows(QModelIndex(), mapFromSourceRow(sourceStart), mapFromSourceRow(sourceEnd),
                  QModelIndex(), mapFromSourceRow(destinationRow));
}

void ExtraListItemProxy::sourceRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(destination)
    Q_UNUSED(row)
    endMoveRows();
}

void ExtraListItemProxy::sourceColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    beginInsertColumns(QModelIndex(), mapFromSourceRow(first), mapFromSourceRow(last));
}

void ExtraListItemProxy::sourceColumnsInserted(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(first)
    Q_UNUSED(last)
    endInsertColumns();
}

void ExtraListItemProxy::sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    beginRemoveColumns(QModelIndex(), mapFromSourceRow(first), mapFromSourceRow(last));
}

void ExtraListItemProxy::sourceColumnsRemoved(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(first)
    Q_UNUSED(last)
    endRemoveColumns();
}

void ExtraListItemProxy::sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_ASSERT(!sourceParent.isValid());
    Q_ASSERT(!destinationParent.isValid());
    beginMoveColumns(QModelIndex(), mapFromSourceRow(sourceStart), mapFromSourceRow(sourceEnd),
                     QModelIndex(), mapFromSourceRow(destinationRow));
}

void ExtraListItemProxy::sourceColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(destination)
    Q_UNUSED(row)
    endMoveColumns();
}

void ExtraListItemProxy::sourceModelAboutToBeReset()
{
    beginResetModel();
}

void ExtraListItemProxy::sourceModelReset()
{
    endResetModel();
}

void ExtraListItemProxy::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
}

void ExtraListItemProxy::sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    emit headerDataChanged(orientation, first, last);
}
