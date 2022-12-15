#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include <QList>

struct SearchResult
{
    SearchResult(const QString &filePath, const QString &lineContent,
                 int lineNumber, int matchStart, int matchLength);

    QString match() const;
    bool matches(const QString &otherLine) const;

    QString filePath;
    QString lineContent;
    int lineNumber;
    int matchStart;
    int matchLength;
    Qt::CheckState checkState = Qt::Checked;
};

struct SearchResultFile
{
    SearchResultFile(const QString &path) : path(path) {}
    ~SearchResultFile();

    Qt::CheckState getCheckState() const;
    int lastLineNumberLength() const;

    void setCheckState(Qt::CheckState state);

    const QString path;
    QList<SearchResult *> results;
};

#endif // SEARCHRESULT_H
