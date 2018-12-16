#ifndef RESOURCEITEMSMODEL_H
#define RESOURCEITEMSMODEL_H

#include "apk/resourcenode.h"
#include <QAbstractItemModel>

class ResourceItemsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum ResourceColumn {
        NodeCaption,
        ResourceLanguage,
        ResourceLocale,
        ResourceApi,
        ResourceQualifiers,
        ResourcePath,
        ColumnCount
    };

    ResourceItemsModel(QObject *parent = nullptr);
    ~ResourceItemsModel() override;

    QModelIndex addNode(ResourceNode *node, const QModelIndex &parent = QModelIndex());

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QString getResourcePath(const QModelIndex &index) const;
    QPixmap getResourceFlag(const QModelIndex &index) const;

private:
    ResourceNode *root;
};

#endif // RESOURCEITEMSMODEL_H
