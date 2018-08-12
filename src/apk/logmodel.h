#ifndef LOGMODEL_H
#define LOGMODEL_H

#include "apk/logentry.h"
#include <QAbstractListModel>

class LogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum LogColumn {
        LogBrief,
        LogDescriptive,
        ColumnCount
    };

    LogModel();
    ~LogModel();

    bool add(LogEntry *entry);
    bool add(const QString &brief, const QString &descriptive, LogEntry::Type type);
    bool add(const QString &brief, const QString &descriptive, const QColor &color);
    void clear();

    void setLoadingState(bool state);
    bool getLoadingState() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    QList<LogEntry *> entries;
    bool isLoading;
};

#endif // LOGMODEL_H
