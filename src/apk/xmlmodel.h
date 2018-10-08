#ifndef XMLRESOURCEMODEL_H
#define XMLRESOURCEMODEL_H

#include "apk/xmlnode.h"
#include <QAbstractItemModel>
#include <QFile>

class XmlResourceModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum ValueColumn {
        Key,
        Value,
        ColumnCount
    };

    XmlResourceModel(const QString &path, QObject *parent = nullptr);
    ~XmlResourceModel();

    bool save(const QString &as = QString()) const;
    QString getFilename() const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

protected:
    XmlNode *addNode(const QDomElement &node);

private:
    QFile *file;
    QDomDocument dom;
    XmlNode *root;
};

#endif // VALUESRESOURCEMODEL_H
