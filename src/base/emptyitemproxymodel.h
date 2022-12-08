#ifndef EMPTYITEMPROXYMODEL_H
#define EMPTYITEMPROXYMODEL_H

#include <QIdentityProxyModel>

class EmptyItemProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    explicit EmptyItemProxyModel(QAbstractItemModel *sourceModel, QObject *parent = nullptr);

    void setEmptyString(const QString &emptyString);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QString emptyString{QChar(0x2014)};
};

#endif // EMPTYITEMPROXYMODEL_H
