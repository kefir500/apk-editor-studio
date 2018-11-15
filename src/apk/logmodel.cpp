#include "apk/logmodel.h"
#include <QBrush>

LogModel::LogModel()
{
    isLoading = false;
}

LogModel::~LogModel()
{
    qDeleteAll(entries);
}

bool LogModel::add(LogEntry *entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
        entries.append(entry);
    endInsertRows();
    return true;
}

bool LogModel::add(const QString &brief, const QString &descriptive, LogEntry::Type type)
{
    LogEntry *entry = new LogEntry(brief, descriptive, type);
    return add(entry);
}

void LogModel::clear()
{
    if (!entries.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
            qDeleteAll(entries);
            entries.clear();
        endRemoveRows();
    }
}

void LogModel::setLoadingState(bool state)
{
    isLoading = state;
}

bool LogModel::getLoadingState() const
{
    return isLoading;
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const int column = index.column();
        const LogEntry *entry = static_cast<LogEntry *>(index.internalPointer());
        if (role == Qt::DisplayRole) {
            switch (column) {
                case LogBrief:       return entry->getBrief();
                case LogDescriptive: return entry->getDescriptive();
            }
        } else if (role == Qt::ForegroundRole) {
            return QBrush(Qt::black);
        } else if (role == Qt::BackgroundRole) {
            return QBrush(entry->getColor());
        }
    }
    return QVariant();
}

QModelIndex LogModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row >= 0 && row < entries.count()) {
        return createIndex(row, column, entries.at(row));
    }
    return QModelIndex();
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return entries.count();
}
