#ifndef MANIFESTMODEL_H
#define MANIFESTMODEL_H

#include "apk/manifest.h"
#include <QAbstractItemModel>
#include <QFile>

class ManifestModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ManifestRow {
        ApplicationLabel,
        VersionCode,
        VersionName,
        MinimumSdk,
        TargetSdk,
        RowCount
    };

    enum ManifestRole {
        ReferenceRole = Qt::UserRole + 1
    };

    explicit ManifestModel(QObject *parent = nullptr);

    void initialize(Manifest *manifest);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    Manifest *manifest;
    QVector<QMap<int, QVariant>> userdata;
};

#endif // MANIFESTMODEL_H
