#include "base/updateitemsmodel.h"
#include "base/apktoolupdateinfo.h"
#include "base/applicationupdateinfo.h"
#include "base/iupdateinfo.h"
#include "base/utils.h"
#include <QIcon>
#include <QNetworkReply>
#include <QNetworkRequest>

UpdateItemsModel::UpdateItemsModel(QObject *parent) : QAbstractTableModel(parent)
{
    auto applicationUpdateInfo = new ApplicationUpdateInfo(this);
    updates.append(applicationUpdateInfo);
    connect(applicationUpdateInfo, &IUpdateInfo::updated, this, [this]() {
        emit dataChanged(index(ApplicationRow, 0), index(ApplicationRow, ColumnCount - 1));
    });

    auto apktoolUpdateInfo = new ApktoolUpdateInfo(this);
    updates.append(apktoolUpdateInfo);
    connect(apktoolUpdateInfo, &IUpdateInfo::updated, this, [this]() {
        emit dataChanged(index(ApktoolRow, 0), index(ApktoolRow, ColumnCount - 1));
    });
}

void UpdateItemsModel::download(int row, QWidget *parent)
{
    updates.at(row)->download(parent);
}

void UpdateItemsModel::refresh()
{
    emit fetching();

    for (const auto update : qAsConst(updates)) {
        update->refresh();
    }

    auto reply = http.get(QNetworkRequest(Utils::getVersionInfoUrl()));
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            const auto metadata = reply->readAll();
            for (const auto update : qAsConst(updates)) {
                update->setMetadata(metadata);
            }
            emit fetched(true);
        } else {
            emit fetched(false, reply->errorString());
        }
        reply->deleteLater();
    });
}

bool UpdateItemsModel::hasUpdates(int row) const
{
    return updates.at(row)->hasUpdates();
}

QString UpdateItemsModel::getWhatsNewUrl(int row) const
{
    return updates.at(row)->getWhatsNewUrl();
}

QVariant UpdateItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        auto updateInfo = updates.at(index.row());
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case ApplicationNameColumn:
                return updateInfo->getApplicationName();
            case CurrentVersionColumn:
                return updateInfo->getCurrentVersion();
            case LatestVersionColumn:
                return updateInfo->getLatestVersion();
            }
        } else if (role == Qt::DecorationRole) {
            if (index.column() == ApplicationNameColumn) {
                return updateInfo->getApplicationIcon();
            }
        } else if (role == Qt::UserRole) {
            switch (index.column()) {
            case WhatsNewUrlColumn:
                return updateInfo->getWhatsNewUrl();
            case HasUpdatesColumn:
                return updateInfo->hasUpdates();
            }
        }
    }
    return {};
}

QVariant UpdateItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ApplicationNameColumn:
            return tr("Application");
        case CurrentVersionColumn:
            return tr("Current Version");
        case LatestVersionColumn:
            return tr("Latest Version");
        }
    }
    return {};
}

QModelIndex UpdateItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row >= 0 && row < RowCount) {
        return createIndex(row, column);
    }
    return {};
}

int UpdateItemsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return RowCount;
}

int UpdateItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}
