#include "base/searchmodel.h"
#include "base/searchresult.h"
#include <QtConcurrent/QtConcurrent>

// SearchModelWorker

void SearchModelWorker::search(const QString &query, const QString &directory)
{
    if (query.isEmpty() || directory.isEmpty()) {
        return;
    }

    QtConcurrent::run([this, query, directory]() {
        searchCancelRequested = false;
        emit searchStarted();
        int resultCount = 0;
        int resultFileCount = 0;
        QMimeDatabase database;
        QDirIterator files(directory, QDir::Files, QDirIterator::Subdirectories);
        while (files.hasNext()) {
            if (searchCancelRequested) {
                break;
            }

            const QString filePath(files.next());
            emit searchProgressed(filePath);

            if (!database.mimeTypeForFile(filePath).inherits("text/plain")) {
                continue;
            }

            QFile file(filePath);
            if (file.open(QFile::ReadOnly)) {
                QTextStream stream(&file);
                stream.setCodec("UTF-8");
                int currentLineNumber = 0;
                bool fileHasResult = false;

                while (!stream.atEnd()) {
                    ++currentLineNumber;
                    const QString line = stream.readLine();
                    int matchOffset = 0;

                    forever {
                        QString match;
                        int matchStart = -1;
                        int matchLength = 0;

                        if (!searchByRegex) {
                            const auto caseSensitivity = searchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
                            matchStart = line.indexOf(query, matchOffset, caseSensitivity);
                            matchLength = query.length();
                        } else {
                            auto regex = QRegularExpression(query);
                            if (!searchCaseSensitive) {
                                regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
                            }
                            const auto match = regex.match(line, matchOffset);
                            matchStart = match.capturedStart(1);
                            matchLength = match.capturedLength(1);
                        }

                        if (matchStart == -1) {
                            break;
                        }

                        emit matchFound(filePath, line, currentLineNumber, matchStart, matchLength);
                        ++resultCount;
                        if (!fileHasResult) {
                            ++resultFileCount;
                            fileHasResult = true;
                        }
                        matchOffset = matchStart + matchLength;
                    }
                }
            }
        }

        emit searchFinished(resultCount, resultFileCount);
    });
}

void SearchModelWorker::cancelSearch()
{
    searchCancelRequested = true;
}

void SearchModelWorker::replace(const QList<SearchResultFile *> &resultFiles, const QString &with)
{
    QtConcurrent::run([this, resultFiles, with]() {
        int totalFilesReplaced = 0;
        int totalResultsReplaced = 0;
        bool success = true;

        replaceCancelRequested = false;
        emit replaceStarted();

        for (const auto &resultFile : resultFiles) {
            if (replaceCancelRequested) {
                break;
            }

            if (resultFile->getCheckState() == Qt::Unchecked) {
                continue;
            }

            const auto filePath = resultFile->path;
            emit replaceProgressed(filePath);

            QFile inputFile(filePath);
            if (!inputFile.open(QFile::ReadOnly)) {
                qWarning() << "Could not open the original match file.";
                success = false;
                continue;
            }
            QTextStream inputStream(&inputFile);
            inputStream.setCodec("UTF-8");

            QTemporaryFile outputFile(filePath);
            if (!outputFile.open()) {
                qWarning() << "Could not open the temporary match file.";
                success = false;
                continue;
            }
            QTextStream outputStream(&outputFile);

            int currentLineNumber = 0;
            QList<SearchResult *> unsavedResults;

            while (!inputStream.atEnd()) {
                auto results = resultFile->results;
                QString line = inputStream.readLine();
                ++currentLineNumber;

                for (auto itResult = results.begin(); itResult != results.end(); ++itResult) {
                    const auto result = *itResult;

                    if (result->lineNumber != currentLineNumber || result->checkState != Qt::Checked) {
                        continue;
                    }

                    if (!result->matches(line)) {
                        // File has been modified, and the search result doesn't match anymore
                        success = false;
                        continue;
                    }

                    const int matchStartOffset = result->match().length() - with.length();

                    line.replace(result->matchStart, result->matchLength, with);

                    // Offset the match ranges for results in the same line
                    for (auto itNextResult = itResult + 1; itNextResult != results.end(); ++itNextResult) {
                        auto nextResult = *itNextResult;
                        if (nextResult->lineNumber != currentLineNumber) {
                            break;
                        }
                        nextResult->lineContent = line;
                        nextResult->matchStart -= matchStartOffset;
                        emit matchUpdated(resultFile, nextResult);
                    }

                    unsavedResults << result;
                }
                outputStream.setCodec(inputStream.codec());
                outputStream << line << Qt::endl;
            }

            if (!inputFile.remove()) {
                qWarning() << "Could not remove the original match file.";
                success = false;
                continue;
            }

            if (outputFile.rename(filePath)) {
                outputFile.setAutoRemove(false);
            } else {
                qWarning() << "Could not move the temporary match file.";
                success = false;
                continue;
            }

            totalFilesReplaced += 1;
            totalResultsReplaced += unsavedResults.count();

            for (const auto &result : qAsConst(unsavedResults)) {
                emit matchReplaced(resultFile, result);
            }
        }

        emit replaceFinished(totalResultsReplaced, totalFilesReplaced, success);
    });
}

void SearchModelWorker::cancelReplace()
{
    replaceCancelRequested = true;
}

void SearchModelWorker::setSearchCaseSensitive(bool enabled)
{
    searchCaseSensitive = enabled;
}

void SearchModelWorker::setSearchByRegex(bool enabled)
{
    searchByRegex = enabled;
}

// SearchModel

SearchModel::SearchModel(QObject *parent) : QAbstractItemModel(parent)
{
    connect(&worker, &SearchModelWorker::searchStarted, this, &SearchModel::searchStarted);
    connect(&worker, &SearchModelWorker::searchProgressed, this, &SearchModel::searchProgressed);
    connect(&worker, &SearchModelWorker::searchFinished, this, &SearchModel::searchFinished);

    connect(&worker, &SearchModelWorker::replaceStarted, this, &SearchModel::replaceStarted);
    connect(&worker, &SearchModelWorker::replaceProgressed, this, &SearchModel::replaceProgressed);
    connect(&worker, &SearchModelWorker::replaceFinished, this, &SearchModel::replaceFinished);

    connect(&worker, &SearchModelWorker::matchFound, this, &SearchModel::add);
    connect(&worker, &SearchModelWorker::matchReplaced, this, &SearchModel::remove);
    connect(&worker, &SearchModelWorker::matchUpdated, this, &SearchModel::update);
}

SearchModel::~SearchModel()
{
    clear();
}

bool SearchModel::isResultIndex(const QModelIndex &index)
{
    const int indexType = index.internalId();
    return indexType != RootIndex && indexType != ResultFileIndex;
}

void SearchModel::add(const QString &filePath, const QString &lineContent, int lineNumber, int matchStart, int matchLength)
{
    SearchResult *result = new SearchResult(filePath, lineContent, lineNumber, matchStart, matchLength);
    SearchResultFile *resultFile = nullptr;

    const auto rootIndex = index(0, 0);
    int resultFileRow;

    for (resultFileRow = resultFiles.count() - 1; resultFileRow >= 0; --resultFileRow) {
        auto existingResultFile = resultFiles.at(resultFileRow);
        if (existingResultFile->path == result->filePath) {
            resultFile = existingResultFile;
            break;
        }
    }

    if (resultFiles.isEmpty()) {
        beginInsertRows({}, 0, 0);
        isRootVisible = true;
        endInsertRows();
    }

    if (!resultFile) {
        resultFile = new SearchResultFile(result->filePath);
        const int row = resultFiles.count();
        beginInsertRows(rootIndex, row, row);
        resultFiles.append(resultFile);
        ++totalResultFiles;
    } else {
        const auto resultFileIndex = index(resultFileRow, 0, rootIndex);
        const int row = resultFile->results.count();
        beginInsertRows(resultFileIndex, row, row);
    }

    resultFile->results.append(result);
    endInsertRows();

    ++totalResults;
    emit dataChanged(rootIndex, rootIndex, {Qt::DisplayRole});
}

void SearchModel::remove(SearchResultFile *resultFile, SearchResult *result)
{
    const int resultFileRow = resultFiles.indexOf(resultFile);
    Q_ASSERT(resultFileRow != -1);

    const int resultRow = resultFile->results.indexOf(result);
    Q_ASSERT(resultRow != -1);

    const auto rootIndex = index(0, 0);
    const auto resultFileIndex = index(resultFileRow, 0, rootIndex);
    Q_ASSERT(resultFileIndex.isValid());

    if (resultFile->results.size() > 1) {
        // Remove a single search result
        beginRemoveRows(resultFileIndex, resultRow, resultRow);
        delete resultFile->results.takeAt(resultRow);
        endRemoveRows();
    } else {
        // Remove the whole search result file
        beginRemoveRows(rootIndex, resultFileRow, resultFileRow);
        delete resultFiles.takeAt(resultFileRow);
        --totalResultFiles;
        endRemoveRows();
    }

    if (resultFiles.isEmpty()) {
        beginRemoveRows({}, 0, 0);
        isRootVisible = false;
        endRemoveRows();
    }

    --totalResults;
    emit dataChanged(rootIndex, rootIndex, {Qt::DisplayRole});
}

void SearchModel::update(SearchResultFile *resultFile, SearchResult *result)
{
    const int resultFileRow = resultFiles.indexOf(resultFile);
    Q_ASSERT(resultFileRow != -1);

    const int resultRow = resultFile->results.indexOf(result);
    Q_ASSERT(resultRow != -1);

    const auto rootIndex = index(0, 0);
    const auto resultFileIndex = index(resultFileRow, 0, rootIndex);
    Q_ASSERT(resultFileIndex.isValid());

    const auto resultIndex = index(resultRow, 0, resultFileIndex);
    Q_ASSERT(resultIndex.isValid());

    emit dataChanged(resultIndex, resultIndex);
}

void SearchModel::clear()
{
    beginResetModel();
    qDeleteAll(resultFiles);
    resultFiles.clear();
    totalResults = 0;
    totalResultFiles = 0;
    isRootVisible = false;
    endResetModel();
}

void SearchModel::search(const QString &query, const QString &directory)
{
    if (query.isEmpty() || directory.isEmpty()) {
        return;
    }

    clear();
    worker.search(query, directory);
}

void SearchModel::cancelSearch()
{
    worker.cancelSearch();
}

void SearchModel::replace(const QString &with)
{
    worker.replace(resultFiles, with);
}

void SearchModel::cancelReplace()
{
    worker.cancelReplace();
}

Qt::CheckState SearchModel::getRootCheckState() const
{
    bool hasCheckedItems = false;
    bool hasUncheckedItems = false;

    for (const auto *resultFile : resultFiles) {
        const auto fileCheckState = resultFile->getCheckState();

        if (fileCheckState == Qt::Checked) {
            hasCheckedItems = true;
        } else if (fileCheckState == Qt::Unchecked) {
            hasUncheckedItems = true;
        }

        if (fileCheckState == Qt::PartiallyChecked || (hasCheckedItems && hasUncheckedItems)) {
            return Qt::PartiallyChecked;
        }
    }

    Q_ASSERT((hasCheckedItems != hasUncheckedItems) || resultFiles.isEmpty());
    return hasCheckedItems ? Qt::Checked : Qt::Unchecked;
}

void SearchModel::setRootPath(const QString &path)
{
    rootPath = path + '/';
}

void SearchModel::setSearchCaseSensitive(bool enabled)
{
    worker.setSearchCaseSensitive(enabled);
}

void SearchModel::setSearchByRegex(bool enabled)
{
    worker.setSearchByRegex(enabled);
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const int row = index.row();
    const int indexType = index.internalId();

    if (indexType == RootIndex) {
        switch (role) {
        case Qt::DisplayRole:
            //: "%1" and "%2" will be replaced with arbitrary numbers representing the search results.
            return tr("%1 result(s) in %2 file(s)").arg(totalResults).arg(totalResultFiles);
        case Qt::CheckStateRole:
            return getRootCheckState();
        }

        return QVariant();
    }

    if (indexType == ResultFileIndex) {
        if (row >= resultFiles.count()) {
            return QVariant();
        }

        const auto resultFile = resultFiles.at(row);
        Q_ASSERT(resultFile);

        switch (role) {
        case Qt::DisplayRole: {
            const QString caption = QString("%1 (%2)").arg(resultFile->path).arg(resultFile->results.count());
            if (caption.startsWith(rootPath)) {
                return caption.mid(rootPath.length());
            }
            return caption;
        }
        case Qt::CheckStateRole:
            return resultFile->getCheckState();
        case FilePathRole:
            return resultFile->path;
        }

        return QVariant();
    }

    const int parentRow = index.internalId();
    if (parentRow >= resultFiles.count()) {
        return QVariant();
    }

    const auto resultFile = resultFiles.at(parentRow);
    Q_ASSERT(resultFile);

    if (row >= resultFile->results.count()) {
        return QVariant();
    }

    const auto &result = resultFile->results.at(row);

    switch (role) {
    case Qt::DisplayRole:
        return result->lineContent;
    case Qt::CheckStateRole:
        return result->checkState;
    case FilePathRole:
        return result->filePath;
    case LineNumberRole:
        return result->lineNumber;
    case LineNumberLengthRole:
        return resultFile->lastLineNumberLength();
    case MatchStartRole:
        return result->matchStart;
    case MatchLengthRole:
        return result->matchLength;
    }

    return QVariant();
}

bool SearchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        const int row = index.row();
        const auto rootIndex = this->index(0, 0);

        switch (static_cast<int>(index.internalId())) {
        case RootIndex: {
            for (int i = 0; i < resultFiles.count(); ++i) {
                auto resultFile = resultFiles.at(i);
                resultFile->setCheckState(static_cast<Qt::CheckState>(value.toInt()));
                const auto resultFileIndex = this->index(i, 0, rootIndex);
                emit dataChanged(
                    this->index(0, 0, resultFileIndex),
                    this->index(resultFile->results.count() - 1, 0, resultFileIndex),
                    {Qt::CheckStateRole}
                );
            }
            emit dataChanged(
                this->index(0, 0, index),
                this->index(resultFiles.count() - 1, 0, index),
                {Qt::CheckStateRole}
            );
            emit dataChanged(index, index, {Qt::CheckStateRole});
            return true;
        }
        case ResultFileIndex: {
            const auto resultFile = resultFiles.at(row);
            Q_ASSERT(resultFile);
            resultFile->setCheckState(static_cast<Qt::CheckState>(value.toInt()));
            const auto firstChildIndex = this->index(0, 0, index);
            const auto lastChildIndex = this->index(resultFile->results.count() - 1, 0, index);
            emit dataChanged(firstChildIndex, lastChildIndex, {Qt::CheckStateRole});
            emit dataChanged(index, index, {Qt::CheckStateRole});
            emit dataChanged(rootIndex, rootIndex, {Qt::CheckStateRole});
            return true;
        }
        default: {
            const auto parentRow = index.internalId();
            const auto resultFile = resultFiles.at(parentRow);
            Q_ASSERT(resultFile);
            resultFile->results[row]->checkState = static_cast<Qt::CheckState>(value.toInt());
            emit dataChanged(index, index, {Qt::CheckStateRole});
            emit dataChanged(index.parent(), index.parent(), {Qt::CheckStateRole});
            emit dataChanged(rootIndex, rootIndex, {Qt::CheckStateRole});
            return true;
        }
        }
    }

    return false;
}

Qt::ItemFlags SearchModel::flags(const QModelIndex &index) const
{
    const auto flags = QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
    return isResultIndex(index) ? flags | Qt::ItemNeverHasChildren : flags;
}

QModelIndex SearchModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(row, column, RootIndex);
    }

    const int indexType = parent.internalId();

    if (indexType == RootIndex) {
        return createIndex(row, column, ResultFileIndex);
    }

    if (indexType == ResultFileIndex) {
        return createIndex(row, column, parent.row());
    }

    return {};
}

QModelIndex SearchModel::parent(const QModelIndex &index) const
{
    const int indexType = index.internalId();

    if (indexType == RootIndex) {
        return {};
    }

    if (indexType == ResultFileIndex) {
        return createIndex(0, 0, RootIndex);
    }

    const int parentRow = index.internalId();
    return createIndex(parentRow, 0, ResultFileIndex);
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return isRootVisible;
    }

    const int indexType = parent.internalId();

    if (indexType == RootIndex) {
        return resultFiles.count();
    }

    if (indexType == ResultFileIndex) {
        const auto resultFile = resultFiles.at(parent.row());
        Q_ASSERT(resultFile);
        return resultFile->results.count();
    }

    return 0;
}

int SearchModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}
