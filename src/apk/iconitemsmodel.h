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

    enum IconType {
        Icon,
        RoundIcon,
        Banner
    };

    explicit IconItemsModel(QObject *parent = nullptr);
    ~IconItemsModel() override;

    bool addIcon(const QPersistentModelIndex &index, IconType type = Icon);
    QIcon getIcon() const;
    QPixmap getPixmap(const QModelIndex &index) const;
    QString getIconPath(const QModelIndex &index) const;
    QString getIconCaption(const QModelIndex &index) const;

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
    class IconItem;

    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    QList<IconItem *> icons;
    QMap<QPersistentModelIndex, IconItem *> sourceToProxyMap;
};

class IconItemsModel::IconItem
{
public:
    IconItem(const QPersistentModelIndex &index) : index(index) {}

    const QPersistentModelIndex &getIndex() const;

    IconType getType() const;
    void setType(IconType type);

private:
    QPersistentModelIndex index;

    IconType type;
};

#endif // ICONITEMSMODEL_H
