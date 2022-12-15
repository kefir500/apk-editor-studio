#ifndef SEARCHSHEET_H
#define SEARCHSHEET_H

#include "sheets/basesheet.h"

class ElidedLabel;
class QLabel;
class QLineEdit;
class QPushButton;
class SearchModel;
class SearchResultView;

class SearchSheet : public BaseSheet
{
    Q_OBJECT

public:
    explicit SearchSheet(QWidget *parent = nullptr);

    bool finalize() override;
    void setSearchPath(const QString &path);

signals:
    void editRequested(const QString &filePath, int lineNumber, int columnNumber, int selectionLength);

protected:
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    enum State {
        StateIdle,
        StateSearching,
        StateReplacing,
    };

    void updateState(State state);
    void updateSearchStats(int resultCount, int fileCount);
    void updateReplaceStats(int resultCount, int fileCount);
    void retranslate();

    QString searchPath;
    SearchModel *searchModel;

    QLabel *searchLabel;
    QLineEdit *searchInput;
    QPushButton *searchButton;
    QPushButton *searchStopButton;
    QLabel *replaceLabel;
    QLineEdit *replaceInput;
    QPushButton *replaceButton;
    QPushButton *replaceStopButton;
    SearchResultView *resultsView;
    ElidedLabel *statusLabel;
};

#endif // SEARCHSHEET_H
