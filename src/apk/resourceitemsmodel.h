#ifndef RESOURCEITEMSMODEL_H
#define RESOURCEITEMSMODEL_H

#include "apk/iresourceitemsmodel.h"
#include <QAbstractItemModel>
#include <QFileIconProvider>
#include <QFuture>

class Project;
class ResourceFile;
class ResourceNode;

class ResourceItemsModel : public QAbstractItemModel, public IResourceItemsModel
{
    Q_OBJECT
    Q_INTERFACES(IResourceItemsModel)

public:
    enum Column {
        CaptionColumn,
        LanguageColumn,
        LocaleColumn,
        DpiColumn,
        ApiColumn,
        QualifiersColumn,
        PathColumn,
        ColumnCount
    };

    enum Role {
        SortRole = Qt::UserRole + 1
    };

    ResourceItemsModel(QObject *parent = nullptr);
    ~ResourceItemsModel() override;

    QFuture<void> initialize(const QString &path);
    QModelIndex addNode(ResourceNode *node, const QModelIndex &parent = QModelIndex());
    bool replaceResource(const QModelIndex &index, const QString &file = QString(), QWidget *parent = nullptr) override;
    bool removeResource(const QModelIndex &index) override;
    QString getResourcePath(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QModelIndex findIndex(const QString &path) const;
    QModelIndex findIndex(const QString &path, const QModelIndex &parent) const;
    const ResourceFile *getResourceFile(const QModelIndex &index) const;

private:
    ResourceNode *root;
    QFileIconProvider iconProvider;
};

#endif // RESOURCEITEMSMODEL_H
