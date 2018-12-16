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
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    QMap<QPersistentModelIndex, int> sourceToProxyMap;
    QMap<int, QPersistentModelIndex> proxyToSourceMap;
};

#endif // ICONITEMSMODEL_H
