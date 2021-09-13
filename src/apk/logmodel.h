#ifndef LOGMODEL_H
#define LOGMODEL_H

#include "apk/logentry.h"
#include <QAbstractListModel>

class LogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Column {
        BriefColumn,
        DescriptiveColumn,
        ColumnCount
    };

    LogModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}
    ~LogModel() override;

    QModelIndex add(LogEntry *entry);
    QModelIndex add(const QString &brief, LogEntry::Type type = LogEntry::Information);
    QModelIndex add(const QString &brief, const QString &descriptive, LogEntry::Type type = LogEntry::Information);
    void update(const QModelIndex &index, const QString &brief, const QString &descriptive = {}, LogEntry::Type type = LogEntry::Information);
    void remove(const QModelIndex &index);
    void clear();

    bool hasLoadingEntries() const;
    void setExclusiveLoading(bool exclusive);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

signals:
    void added(LogEntry *entry);
    void loadingStateChanged(bool state);

private:
    QList<LogEntry *> entries;
    bool exclusiveLoading = false;
};

#endif // LOGMODEL_H
