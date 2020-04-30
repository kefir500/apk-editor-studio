#ifndef XMLRESOURCEMODEL_H
#define XMLRESOURCEMODEL_H

#include "apk/xmlnode.h"
#include <QAbstractItemModel>
#include <QFile>

class XmlResourceModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Column {
        KeyColumn,
        ValueColumn,
        ColumnCount
    };

    XmlResourceModel(const QString &path, QObject *parent = nullptr);
    ~XmlResourceModel() override;

    bool save(const QString &as = QString()) const;
    QString getFilename() const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

protected:
    XmlNode *addNode(const QDomElement &node);

private:
    QFile *file;
    QDomDocument dom;
    XmlNode *root;
};

#endif // VALUESRESOURCEMODEL_H
