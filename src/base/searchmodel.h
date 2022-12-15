#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include "base/searchresult.h"
#include <QAbstractItemModel>

class SearchModelWorker : public QObject
{
    Q_OBJECT

public:
    void search(const QString &query, const QString &directory);
    void cancelSearch();

    void replace(const QList<SearchResultFile *> &resultFiles, const QString &with);
    void cancelReplace();

    void setSearchCaseSensitive(bool enabled);
    void setSearchByRegex(bool enabled);

signals:
    void searchStarted();
    void searchProgressed(const QString &currentFile);
    void searchFinished(int resultCount, int fileCount);

    void replaceStarted();
    void replaceProgressed(const QString &currentFile);
    void replaceFinished(int resultCount, int fileCount, bool allSucceeded);

    void matchFound(const QString &filePath, const QString &lineContent, int lineNumber, int matchStart, int matchLength);
    void matchReplaced(SearchResultFile *resultFile, SearchResult *result);
    void matchUpdated(SearchResultFile *resultFile, SearchResult *result);

private:
    bool searchCaseSensitive = false;
    bool searchByRegex = false;

    bool searchCancelRequested = true;
    bool replaceCancelRequested = true;
};

class SearchModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Column {
        LineNumberColumn,
        LineContentColumn,
        ColumnCount
    };

    enum Role {
        FilePathRole = Qt::UserRole + 1,
        LineNumberRole,
        LineNumberLengthRole,
        MatchStartRole,
        MatchLengthRole
    };

    explicit SearchModel(QObject *parent = nullptr);
    ~SearchModel() override;

    static bool isResultIndex(const QModelIndex &index);

    void add(const QString &filePath, const QString &lineContent, int lineNumber, int matchStart, int matchLength);
    void remove(SearchResultFile *resultFile, SearchResult *result);
    void update(SearchResultFile *resultFile, SearchResult *result);
    void clear();

    void search(const QString &query, const QString &directory);
    void cancelSearch();

    void replace(const QString &with);
    void cancelReplace();

    Qt::CheckState getRootCheckState() const;
    void setRootPath(const QString &path);

    void setSearchCaseSensitive(bool enabled);
    void setSearchByRegex(bool enabled);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

signals:
    void searchStarted();
    void searchProgressed(const QString &currentFile);
    void searchFinished(int resultCount, int fileCount);

    void replaceStarted();
    void replaceProgressed(const QString &currentFile);
    void replaceFinished(int resultCount, int fileCount, bool allSucceeded);

private:
    enum IndexType {
        RootIndex = -1,
        ResultFileIndex = -2,
    };

    SearchModelWorker worker;

    QList<SearchResultFile *> resultFiles;
    int totalResults = 0;
    int totalResultFiles = 0;

    QString rootPath;
    bool isRootVisible = false;
};

#endif // SEARCHMODEL_H
