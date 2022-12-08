#ifndef UPDATEITEMSMODEL_H
#define UPDATEITEMSMODEL_H

#include <QAbstractTableModel>
#include <QNetworkAccessManager>

class IUpdateInfo;

class UpdateItemsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Row {
        ApplicationRow,
        ApktoolRow,
        RowCount
    };

    enum Column {
        ApplicationNameColumn,
        CurrentVersionColumn,
        LatestVersionColumn,
        WhatsNewUrlColumn,
        HasUpdatesColumn,
        ColumnCount
    };

    UpdateItemsModel(QObject *parent = nullptr);

    void download(int row, QWidget *parent = nullptr);
    void refresh();
    bool hasUpdates(int row) const;
    QString getWhatsNewUrl(int row) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

signals:
    void fetching();
    void fetched(bool success, const QString &error = {});

private:
    QList<IUpdateInfo *> updates;
    QNetworkAccessManager http;
};

#endif // UPDATEITEMSMODEL_H
