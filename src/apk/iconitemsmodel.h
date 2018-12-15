#ifndef ICONITEMSMODEL_H
#define ICONITEMSMODEL_H

#include <QAbstractProxyModel>
#include "apk/resourceitemsmodel.h"

class IconItemsModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    enum IconColumn {
        IconCaption,
        IconPath,
        ColumnCount
    };

    explicit IconItemsModel(QObject *parent = nullptr);

    bool addIcon(const QPersistentModelIndex &index);
    QIcon getIcon() const;
    QString getIconPath(const QModelIndex &index) const;

    ResourceItemsModel *sourceModel() const;
    void setSourceModel(ResourceItemsModel *sourceModel);
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

#endif // ICONITEMSMODEL_H
