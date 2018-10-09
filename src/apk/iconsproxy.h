#ifndef ICONSPROXY_H
#define ICONSPROXY_H

#include <QAbstractProxyModel>
#include "apk/resourcesmodel.h"

class IconsProxy : public QAbstractProxyModel
{
    Q_OBJECT

public:
    enum IconColumn {
        IconCaption,
        IconPath,
        ColumnCount
    };

    explicit IconsProxy(QObject *parent = nullptr);

    bool addIcon(const QPersistentModelIndex &index);
    QString getIconPath(const QModelIndex &index) const;

    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    QMap<QPersistentModelIndex, int> sourceToProxyMap;
    QMap<int, QPersistentModelIndex> proxyToSourceMap;
};

#endif // ICONSPROXY_H
