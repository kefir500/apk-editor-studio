#ifndef TITLEITEMSMODEL_H
#define TITLEITEMSMODEL_H

#include "apk/package.h"
#include "apk/titlenode.h"
#include <QAbstractTableModel>

class TitleItemsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ValueColumn,
        LanguageColumn,
        QualifiersColumn,
        PathColumn,
        ColumnCount
    };

    explicit TitleItemsModel(const Package *apk, QObject *parent = nullptr);
    ~TitleItemsModel() override;

    void save() const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

signals:
    void initialized();

private:
    QList<TitleNode *> nodes;
};

#endif // TITLEITEMSMODEL_H
