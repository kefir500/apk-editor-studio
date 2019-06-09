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
        ResourceDpi,
        ResourceApi,
        ResourceQualifiers,
        ResourcePath,
        ColumnCount
    };

    enum ResourceRole {
        ResourceNameRole = Qt::UserRole + 1,
        ResourceTypeRole,
        SortRole
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
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool replaceResource(const QModelIndex &index, const QString &file);
    bool removeResource(const QString &path);
    QModelIndex findIndex(const QString &path, const QModelIndex &parent) const;
    const ResourceFile *getResource(const QModelIndex &index) const;
    const Project *getApk() const;

signals:
    void added(const QModelIndex &index);

private:
    const Project *apk;
    ResourceNode *root;
};

#endif // RESOURCEITEMSMODEL_H
