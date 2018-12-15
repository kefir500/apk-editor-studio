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
    ~ResourceItemsModel() Q_DECL_OVERRIDE;

    QModelIndex addNode(ResourceNode *node, const QModelIndex &parent = QModelIndex());

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QString getResourcePath(const QModelIndex &index) const;
    QPixmap getResourceFlag(const QModelIndex &index) const;

private:
    ResourceNode *root;
};

#endif // RESOURCEITEMSMODEL_H
