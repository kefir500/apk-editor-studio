#ifndef TITLEITEMSMODEL_H
#define TITLEITEMSMODEL_H

#include "apk/project.h"
#include "apk/titlenode.h"
#include <QAbstractTableModel>

class TitleItemsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum TitleColumn {
        Value,
        ResourceLanguage,
        ResourceQualifiers,
        ResourcePath,
        ColumnCount
    };

    explicit TitleItemsModel(const Project *apk, QObject *parent = nullptr);
    ~TitleItemsModel() Q_DECL_OVERRIDE;

    bool save() const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    void add(const QString &filepath, const QString &key);

    QList<TitleNode *> nodes;
};

#endif // TITLEITEMSMODEL_H
