#include "base/searchresult.h"

SearchResult::SearchResult(const QString &filePath, const QString &lineContent,
                           int lineNumber, int matchStart, int matchLength)
    : filePath(filePath)
    , lineContent(lineContent)
    , lineNumber(lineNumber)
    , matchStart(matchStart)
    , matchLength(matchLength)
{}

QString SearchResult::match() const
{
    return lineContent.mid(matchStart, matchLength);
}

bool SearchResult::matches(const QString &otherLine) const
{
    return match() == otherLine.mid(matchStart, matchLength);
}

SearchResultFile::~SearchResultFile()
{
    qDeleteAll(results);
}

Qt::CheckState SearchResultFile::getCheckState() const
{
    bool hasCheckedItems = false;
    bool hasUncheckedItems = false;

    for (const auto *result : results) {
        if (result->checkState == Qt::Checked) {
            hasCheckedItems = true;
        } else if (result->checkState == Qt::Unchecked) {
            hasUncheckedItems = true;
        }

        if (hasCheckedItems && hasUncheckedItems) {
            return Qt::PartiallyChecked;
        }
    }

    Q_ASSERT((hasCheckedItems != hasUncheckedItems) || results.isEmpty());
    return hasCheckedItems ? Qt::Checked : Qt::Unchecked;
}

int SearchResultFile::lastLineNumberLength() const
{
    if (results.isEmpty()) {
        return 0;
    }
    return QString::number(results.last()->lineNumber).length();
}

void SearchResultFile::setCheckState(Qt::CheckState state)
{
    for (auto &result : results) {
        result->checkState = state;
    }
}
