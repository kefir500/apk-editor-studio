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

    explicit IconItemsModel(QObject *parent = nullptr) : QAbstractProxyModel(parent) {}
    ~IconItemsModel() override;

    void setSourceModel(ResourceItemsModel *sourceModel);

    QIcon getIcon() const;
    QIcon getIcon(const QModelIndex &index) const;
    QString getIconPath(const QModelIndex &index) const;
    QString getIconCaption(const QModelIndex &index) const;

    ResourceItemsModel *sourceModel() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    struct IconItem
    {
        IconItem(const QPersistentModelIndex &index, IconType type) : index(index), type(type) {}
        const QPersistentModelIndex index;
        const IconType type;
    };

    bool addIcon(const QPersistentModelIndex &index, IconType type = Icon);
    void onResourceAdded(const QModelIndex &index);
    void onResourceRemoved(const QModelIndex &index);
    void onResourceChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    const Project *apk();

    QList<IconItem *> icons;
    QMap<QPersistentModelIndex, IconItem *> sourceToProxyMap;
};

#endif // ICONITEMSMODEL_H
