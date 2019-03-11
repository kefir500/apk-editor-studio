#ifndef RESOURCEITEMSMODEL_H
#define RESOURCEITEMSMODEL_H

#include "apk/resourcenode.h"
#include <QAbstractItemModel>

class Project;

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
        ResourceName,
        ResourceType,
        ResourcePath,
        ColumnCount
    };

    ResourceItemsModel(const Project *apk, QObject *parent = nullptr);
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
    QString getResourceName(const QModelIndex &index) const;
    QString getResourceType(const QModelIndex &index) const;
    const Project *getApk() const;

signals:
    void added(const QModelIndex &index);
    void removed(const QModelIndex &index);

private:
    const Project *apk;
    ResourceNode *root;
};

#endif // RESOURCEITEMSMODEL_H
