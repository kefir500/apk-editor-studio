#ifndef MANIFESTMODEL_H
#define MANIFESTMODEL_H

#include "apk/manifest.h"
#include <QAbstractItemModel>
#include <QFile>

class ManifestModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Row {
        ApplicationLabelRow,
        VersionCodeRow,
        VersionNameRow,
        MinimumSdkRow,
        TargetSdkRow,
        RowCount
    };

    enum Role {
        ReferenceRole = Qt::UserRole + 1
    };

    explicit ManifestModel(QObject *parent = nullptr);

    void initialize(Manifest *manifest);

    QString getApplicationLabel() const;
    int getVersionCode() const;
    QString getVersionName() const;
    int getMinimumSdk() const;
    int getTargetSdk() const;

    bool setApplicationLabel(const QString &label);
    bool setVersionCode(int version);
    bool setVersionName(const QString &version);
    bool setMinimumSdk(int sdk);
    bool setTargetSdk(int sdk);

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
