#include "apk/logmodel.h"
#include <QPalette>

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
    emit added(entry);
    return true;
}

bool LogModel::add(const QString &brief, const QString &descriptive, LogEntry::Type type)
{
    auto entry = new LogEntry(brief, descriptive, type);
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
    emit loadingStateChanged(state);
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
            case BriefColumn:
                return entry->getBrief();
            case DescriptiveColumn:
                return entry->getDescriptive();
            }
        } else if (role == Qt::ForegroundRole) {
            return QBrush(QPalette().color(QPalette::Text));
        } else if (role == Qt::BackgroundRole) {
            return QBrush(entry->getColor());
        }
    }
    return QVariant();
}

QModelIndex LogModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row >= 0 && row < entries.count()) {
        return createIndex(row, column, entries.at(row));
    }
    return QModelIndex();
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return entries.count();
}
