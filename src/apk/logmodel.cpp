#include "apk/logmodel.h"
#include <QPalette>

LogModel::~LogModel()
{
    qDeleteAll(entries);
}

QModelIndex LogModel::add(LogEntry *entry)
{
    if (exclusiveLoading && !entries.isEmpty()) {
        entries.last()->setLoading(false);
    }
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
        entries.append(entry);
    endInsertRows();
    emit added(entry);
    emit loadingStateChanged(hasLoadingEntries());
    return createIndex(entries.count() - 1, 0, entry);
}

QModelIndex LogModel::add(const QString &brief, LogEntry::Type type)
{
    return add(new LogEntry(brief, type));
}

QModelIndex LogModel::add(const QString &brief, const QString &descriptive, LogEntry::Type type)
{
    return add(new LogEntry(brief, descriptive, type));
}

void LogModel::update(const QModelIndex &index, const QString &brief, const QString &descriptive, LogEntry::Type type)
{
    Q_ASSERT(index.isValid());
    auto entry = static_cast<LogEntry *>(index.internalPointer());
    entry->setBrief(brief);
    entry->setDescriptive(descriptive);
    entry->setType(type);
    emit dataChanged(index, index);
    emit loadingStateChanged(hasLoadingEntries());
}

void LogModel::remove(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    auto entry = static_cast<LogEntry *>(index.internalPointer());
    beginRemoveRows(QModelIndex(), index.row(), index.row());
        delete entry;
    endRemoveRows();
    emit loadingStateChanged(hasLoadingEntries());
}

void LogModel::clear()
{
    if (!entries.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
            qDeleteAll(entries);
            entries.clear();
        endRemoveRows();
    }
    emit loadingStateChanged(false);
}

bool LogModel::hasLoadingEntries() const
{
    for (const auto entry : entries) {
        if (entry->getLoading()) {
            return true;
        }
    }
    return false;
}

void LogModel::setExclusiveLoading(bool exclusive)
{
    exclusiveLoading = exclusive;
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
