#ifndef EXTRALISTITEMPROXY_H
#define EXTRALISTITEMPROXY_H

#include <QAbstractProxyModel>

class ExtraListItemProxy : public QAbstractProxyModel
{
public:
    explicit ExtraListItemProxy(QObject *parent = nullptr) : QAbstractProxyModel(parent) {}

    void appendRow();
    void appendRows(int count);
    void prependRow();
    void prependRows(int count);

    bool isRowAppended(int row) const;
    bool isRowPrepended(int row) const;
    int mapFromSourceRow(int row) const;
    int mapToSourceRow(int row) const;

    void setSourceModel(QAbstractItemModel *sourceModel) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    void sourceRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void sourceRowsInserted(const QModelIndex &parent, int first, int last);
    void sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void sourceRowsRemoved(const QModelIndex &parent, int first, int last);
    void sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void sourceRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void sourceColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void sourceColumnsInserted(const QModelIndex &parent, int first, int last);
    void sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void sourceColumnsRemoved(const QModelIndex &parent, int first, int last);
    void sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void sourceColumnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void sourceModelAboutToBeReset();
    void sourceModelReset();
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);
    void sourceLayoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);
    void sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);

    QList<QMap<int, QVariant>> appendedRows;
    QList<QMap<int, QVariant>> prependedRows;
};

#endif // EXTRALISTITEMPROXY_H
