#include "base/emptyitemproxymodel.h"

EmptyItemProxyModel::EmptyItemProxyModel(QAbstractItemModel *sourceModel, QObject *parent)
    : QIdentityProxyModel(parent)
{
    setSourceModel(sourceModel);
}

void EmptyItemProxyModel::setEmptyString(const QString &emptyString)
{
    this->emptyString = emptyString;
}

QVariant EmptyItemProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && QIdentityProxyModel::data(index, role).isNull()) {
        return emptyString;
    }
    return QIdentityProxyModel::data(index, role);
}
