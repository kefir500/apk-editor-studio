#include "widgets/codesearchbar.h"
#include "widgets/codeeditor.h"
#include "base/application.h"
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

CodeSearchBar::CodeSearchBar(CodeEditor *parent)
    : QWidget(parent)
    , editor(parent)
{
    hide();

    auto layout = new QFormLayout(this);
    layout->setMargin(6);

    // Find

    findBar = new QWidget(this);
    findLabel = new QLabel(this);
    layout->addRow(findLabel, findBar);
    auto findLayout = new QHBoxLayout(findBar);
    findLayout->setMargin(0);

    findInput = new QLineEdit(this);
    connect(findInput, &QLineEdit::textChanged, editor, &CodeEditor::setSearchQuery);
    findLayout->addWidget(findInput);

    resultsLabel = new QLabel(this);
    findLayout->addWidget(resultsLabel);

    auto btnFindPrevious = new QToolButton(this);
    btnFindPrevious->setDefaultAction(app->actions.getFindPrevious(this));
    connect(btnFindPrevious, &QToolButton::clicked, editor, &CodeEditor::prevSearchQuery);
    findLayout->addWidget(btnFindPrevious);

    auto btnFindNext = new QToolButton(this);
    btnFindNext->setDefaultAction(app->actions.getFindNext(this));
    connect(btnFindNext, &QToolButton::clicked, editor, [=]() {
        editor->nextSearchQuery(true);
    });
    findLayout->addWidget(btnFindNext);

    btnOpenReplaceBar = new QToolButton(this);
    btnOpenReplaceBar->setDefaultAction(app->actions.getReplace(this));
    connect(btnOpenReplaceBar, &QToolButton::clicked, this, [=]() {
        showReplaceBar();
        if (!findInput->text().isEmpty()) {
            focusReplaceBar();
        }
    });
    findLayout->addWidget(btnOpenReplaceBar);

    btnClose = new QToolButton(this);
    btnClose->setIcon(QIcon::fromTheme("dialog-close"));
    connect(btnClose, &QToolButton::clicked, this, &CodeSearchBar::hide);
    findLayout->addWidget(btnClose);

    // Replace

    replaceBar = new QWidget(this);
    replaceBar->hide();
    replaceLabel = new QLabel(this);
    replaceLabel->hide();
    auto replaceLayout = new QHBoxLayout(replaceBar);
    replaceLayout->setMargin(0);

    replaceInput = new QLineEdit(this);
    replaceLayout->addWidget(replaceInput);

    btnReplace = new QToolButton(this);
    btnReplace->setIcon(QIcon::fromTheme("edit-find-replace"));
    connect(btnReplace, &QToolButton::clicked, this, [=]() {
        editor->replaceOne(replaceInput->text());
    });
    connect(replaceInput, &QLineEdit::returnPressed, btnReplace, &QToolButton::click);
    replaceLayout->addWidget(btnReplace);

    btnReplaceAll = new QToolButton(this);
    btnReplaceAll->setIcon(QIcon::fromTheme("edit-find-replace-all"));
    connect(btnReplaceAll, &QToolButton::clicked, this, [=]() {
        editor->replaceAll(replaceInput->text());
    });
    replaceLayout->addWidget(btnReplaceAll);

    btnCloseReplaceBar = new QToolButton(this);
    btnCloseReplaceBar->setIcon(QIcon::fromTheme("view-close"));
    connect(btnCloseReplaceBar, &QToolButton::clicked, this, [=]() {
        hideReplaceBar();
        focusFindBar();
    });
    replaceLayout->addWidget(btnCloseReplaceBar);

    setTabOrder(findInput, replaceInput);
    retranslate();
}

void CodeSearchBar::setResults(int totalResults, int currentResult)
{
    this->totalResults = totalResults;
    if (currentResult) {
        this->currentResult = currentResult;
    }
    updateResults();
}

void CodeSearchBar::setFindText(const QString &text)
{
    findInput->blockSignals(true);
    findInput->setText(text);
    findInput->blockSignals(false);
    emit findInput->textChanged(text);
}

void CodeSearchBar::focusFindBar()
{
    findInput->setFocus();
    findInput->selectAll();
}

void CodeSearchBar::focusReplaceBar()
{
    replaceInput->setFocus();
    replaceInput->selectAll();
}

void CodeSearchBar::showReplaceBar()
{
    if (replaceBar->isHidden()) {
        qobject_cast<QFormLayout *>(layout())->addRow(replaceLabel, replaceBar);
        replaceBar->show();
        replaceLabel->show();
        btnCloseReplaceBar->show();
        btnOpenReplaceBar->hide();
    }
}

void CodeSearchBar::hideReplaceBar()
{
    if (replaceBar->isVisible()) {
        qobject_cast<QFormLayout *>(layout())->takeRow(replaceBar);
        replaceBar->hide();
        replaceLabel->hide();
        btnCloseReplaceBar->hide();
        btnOpenReplaceBar->show();
    }
}

void CodeSearchBar::updateResults()
{
    if (findInput->text().isEmpty()) {
        resultsLabel->hide();
        return;
    }
    resultsLabel->setText(totalResults > 0
        ? QString(layoutDirection() == Qt::LeftToRight ? "%1/%2" : "%2/%1").arg(currentResult).arg(totalResults)
        : tr("No results"));
    resultsLabel->show();
}

void CodeSearchBar::changeEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange:
        retranslate();
        Q_FALLTHROUGH();
    case QEvent::LayoutDirectionChange:
        updateResults();
        break;
    default:
        break;
    }
    QWidget::changeEvent(event);
}

void CodeSearchBar::hideEvent(QHideEvent *event)
{
    hideReplaceBar();
    editor->setFocus();
    editor->setSearchQuery("");
    QWidget::hideEvent(event);
}

void CodeSearchBar::showEvent(QShowEvent *event)
{
    editor->setSearchQuery(findInput->text());
    QWidget::showEvent(event);
}

void CodeSearchBar::retranslate()
{
    findLabel->setText(tr("Find:"));
    replaceLabel->setText(tr("Replace:"));
    btnClose->setToolTip(tr("Close"));
    btnReplace->setToolTip("Replace");
    btnReplaceAll->setToolTip("Replace All");
    btnCloseReplaceBar->setToolTip(tr("Close Replace Bar"));
    updateResults();
}
