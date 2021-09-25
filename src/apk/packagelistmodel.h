#ifndef PACKAGELISTMODEL_H
#define PACKAGELISTMODEL_H

#include <QAbstractListModel>

class MainWindow;
class Package;

class PackageListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Column {
        TitleColumn,
        OriginalPathColumn,
        ContentsPathColumn,
        StatusColumn,
        IsUnpackedColumn,
        IsModifiedColumn,
        ColumnCount
    };

    ~PackageListModel() override;

    void add(Package *package);
    bool close(Package *package);

    Package *at(int row) const;
    Package *existing(const QString &filename) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QList<Package *> packages;
};

#endif // PACKAGELISTMODEL_H
