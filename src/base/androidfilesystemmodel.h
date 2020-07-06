#ifndef ANDROIDFILESYSTEMMODEL_H
#define ANDROIDFILESYSTEMMODEL_H

#include "base/androidfilesystemitem.h"
#include <QAbstractTableModel>

class AndroidFileSystemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        NameColumn,
        PathColumn,
        ColumnCount
    };

    enum Role {
        FileTypeRole = Qt::UserRole + 1
    };

    AndroidFileSystemModel(const QString &serial, QObject *parent = nullptr);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QString getItemName(const QModelIndex &index) const;
    QString getItemPath(const QModelIndex &index) const;
    QIcon getItemIcon(const QModelIndex &index) const;
    AndroidFileSystemItem::Type getItemType(const QModelIndex &index) const;

    const QString &getCurrentPath() const;
    void cd(const QString &path);
    void copy(const QString &src, const QString &dst);
    void move(const QString &src, const QString &dst);
    void rename(const QString &src, const QString &dst);
    void remove(const QString &path);
    void download(const QString &src, const QString &dst);
    void upload(const QString &src, const QString &dst);

signals:
    void pathChanged(const QString &path);
    void error(const QString &error);

private:
    void ls();

    QList<AndroidFileSystemItem> fileSystemItems;
    QString serial;
    QString currentPath;
};

#endif // ANDROIDFILESYSTEMMODEL_H
