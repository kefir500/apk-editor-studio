#include "sheets/searchsheet.h"
#include "widgets/elidedlabel.h"
#include "widgets/searchresultview.h"
#include "base/application.h"
#include "base/searchmodel.h"
#include "base/settings.h"
#include <QAction>
#include <QDir>
#include <QEvent>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>

SearchSheet::SearchSheet(QWidget *parent) : BaseSheet(parent)
{
    setSheetIcon(QIcon::fromTheme("edit-find"));

    searchModel = new SearchModel(this);
    connect(searchModel, &SearchModel::searchProgressed, this, [this]() {
        resultsView->expand(searchModel->index(0, 0));
    });

    searchLabel = new QLabel(this);
    searchInput = new QLineEdit(this);
    searchInput->setClearButtonEnabled(true);
    searchButton = new QPushButton(this);
    searchButton->setFocusPolicy(Qt::TabFocus);
    searchStopButton = new QPushButton(this);
    searchStopButton->setFocusPolicy(Qt::TabFocus);
    connect(searchInput, &QLineEdit::returnPressed, searchButton, &QPushButton::click);
    connect(searchButton, &QPushButton::clicked, this, [this]() {
        searchModel->search(searchInput->text(), searchPath);
    });
    connect(searchStopButton, &QPushButton::clicked, this, [this]() {
        searchModel->cancelSearch();
    });

    auto actionCaseSensitive = app->actions.getSearchCaseSensitive(this);
    connect(actionCaseSensitive, &QAction::toggled, this, [this](bool enabled) {
        searchModel->setSearchCaseSensitive(enabled);
        app->settings->setSearchCaseSensitive(enabled);
    });
    actionCaseSensitive->setChecked(app->settings->getSearchCaseSensitive());
    auto btnCaseSensitive = new QToolButton(this);
    btnCaseSensitive->setDefaultAction(actionCaseSensitive);

    auto actionRegex = app->actions.getSearchByRegex(this);
    connect(actionRegex, &QAction::toggled, this, [this](bool enabled) {
        searchModel->setSearchByRegex(enabled);
        app->settings->setSearchByRegex(enabled);
    });
    actionRegex->setChecked(app->settings->getSearchByRegex());
    auto btnRegex = new QToolButton(this);
    btnRegex->setDefaultAction(actionRegex);

    auto searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(btnCaseSensitive);
    searchLayout->addWidget(btnRegex);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(searchStopButton);

    replaceLabel = new QLabel(this);
    replaceInput = new QLineEdit(this);
    replaceInput->setClearButtonEnabled(true);
    replaceButton = new QPushButton(this);
    replaceButton->setFocusPolicy(Qt::TabFocus);
    replaceStopButton = new QPushButton(this);
    replaceStopButton->setFocusPolicy(Qt::TabFocus);
    connect(replaceInput, &QLineEdit::returnPressed, replaceButton, &QPushButton::click);
    connect(replaceButton, &QPushButton::clicked, this, [this]() {
        searchModel->replace(replaceInput->text());
    });
    connect(replaceStopButton, &QPushButton::clicked, this, [this]() {
        searchModel->cancelReplace();
    });
    auto replaceLayout = new QHBoxLayout;
    replaceLayout->addWidget(replaceInput);
    replaceLayout->addWidget(replaceButton);
    replaceLayout->addWidget(replaceStopButton);

    auto controlsLayout = new QFormLayout;
    controlsLayout->addRow(searchLabel, searchLayout);
    controlsLayout->addRow(replaceLabel, replaceLayout);

    resultsView = new SearchResultView(this);
    resultsView->setModel(searchModel);
    connect(resultsView, &SearchResultView::activated, this, [this](const QModelIndex &index) {
        if (!SearchModel::isResultIndex(index)) {
            return;
        }
        const QString filePath = index.data(SearchModel::FilePathRole).toString();
        if (filePath.isEmpty()) {
            return;
        }
        const int lineNumber = index.data(SearchModel::LineNumberRole).toInt();
        const int columnNumber = index.data(SearchModel::MatchStartRole).toInt();
        const int selectionLength = index.data(SearchModel::MatchLengthRole).toInt();
        emit editRequested(filePath, lineNumber, columnNumber, selectionLength);
    });

    statusLabel = new ElidedLabel(this);
    statusLabel->hide();

    connect(searchModel, &SearchModel::searchStarted, this, [this]() {
        updateState(StateSearching);
        statusLabel->show();
    });

    connect(searchModel, &SearchModel::searchProgressed, this, [this](const QString &currentFile) {
        const QString currentSearchPath = currentFile.mid(searchPath.length() + 1);
        //: "%1" will be replaced with a path to the file.
        statusLabel->setText(tr("Searching in %1").arg(currentSearchPath));
    });

    connect(searchModel, &SearchModel::searchFinished, this, [this](int resultCount, int fileCount) {
        updateState(StateIdle);
        updateSearchStats(resultCount, fileCount);
    });

    connect(searchModel, &SearchModel::replaceStarted, this, [this]() {
        updateState(StateReplacing);
        statusLabel->show();
    });

    connect(searchModel, &SearchModel::replaceProgressed, this, [this](const QString &currentFile) {
        const QString currentSearchPath = currentFile.mid(searchPath.length() + 1);
        //: "%1" will be replaced with a path to the file.
        statusLabel->setText(tr("Replacing in %1").arg(currentSearchPath));
    });

    connect(searchModel, &SearchModel::replaceFinished, this, [this](int resultCount, int fileCount, bool success) {
        updateState(StateIdle);
        updateReplaceStats(resultCount, fileCount);
        if (!success) {
            QMessageBox::warning(this, {}, tr("Some occurrences were not replaced."));
        }
    });

    auto layout = new QVBoxLayout(this);
    layout->addLayout(controlsLayout);
    layout->addWidget(resultsView);
    layout->addWidget(statusLabel);

    updateState(StateIdle);
    retranslate();
}

bool SearchSheet::finalize()
{
    searchModel->cancelSearch();
    searchModel->cancelReplace();
    return true;
}

void SearchSheet::setSearchPath(const QString &path)
{
    searchPath = QDir::cleanPath(path);
    searchModel->setRootPath(searchPath);
}

void SearchSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseSheet::changeEvent(event);
}

void SearchSheet::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    searchInput->setFocus();
}

void SearchSheet::updateState(State state)
{
    searchButton->setEnabled(state == StateIdle);
    searchButton->setVisible(state != StateSearching);
    searchStopButton->setVisible(state == StateSearching);
    searchStopButton->setEnabled(state == StateSearching);

    replaceButton->setEnabled(state == StateIdle);
    replaceButton->setVisible(state != StateReplacing);
    replaceStopButton->setVisible(state == StateReplacing);
    replaceStopButton->setEnabled(state == StateReplacing);

    resultsView->setEnabled(state != StateReplacing);
}

void SearchSheet::updateSearchStats(int resultCount, int fileCount)
{
    if (resultCount) {
        //: "%1" and "%2" will be replaced with arbitrary numbers representing the search results.
        statusLabel->setText(tr("Found %1 result(s) in %2 file(s)").arg(resultCount).arg(fileCount));
    } else {
        statusLabel->setText(tr("No results found"));
    }
}

void SearchSheet::updateReplaceStats(int resultCount, int fileCount)
{
    if (resultCount) {
        //: "%1" and "%2" will be replaced with arbitrary numbers representing the search results.
        statusLabel->setText(tr("Replaced %1 occurrence(s) in %2 file(s)").arg(resultCount).arg(fileCount));
    } else {
        statusLabel->setText(tr("Nothing has been replaced"));
    }
}

void SearchSheet::retranslate()
{
    setSheetTitle(tr("Search in Project"));

    searchLabel->setText(tr("Search for:"));
    replaceLabel->setText(tr("Replace with:"));

    searchButton->setText(tr("&Search"));
    replaceButton->setText(tr("&Replace"));

    searchStopButton->setText(tr("Stop"));
    replaceStopButton->setText(tr("Stop"));
}
