#include "sheets/codesheet.h"
#include "widgets/codeeditor.h"
#include "widgets/codesearchbar.h"
#include "base/application.h"
#include <KSyntaxHighlighting/Definition>
#include <QAction>
#include <QBoxLayout>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextCodec>

CodeSheet::CodeSheet(const ResourceModelIndex &index, QWidget *parent) : BaseFileSheet(index, parent)
{
    const QString filename = index.path();
    QString sheetTitle = filename.section('/', -2);
    QRegularExpression guid("^{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}}");
    if (guid.match(sheetTitle).hasMatch()) {
        sheetTitle = sheetTitle.split('/').last();
    }
    setSheetTitle(sheetTitle);
    setSheetIcon(index.icon());

    editor = new CodeEditor(this);
    editor->setDefinition(app->highlightingRepository.definitionForFileName(filename));

    searchBar = new CodeSearchBar(editor);
    connect(editor, &CodeEditor::searchFinished, searchBar, &CodeSearchBar::setResults);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    layout->addWidget(searchBar);
    layout->setMargin(0);
    layout->setSpacing(0);

    file = new QFile(filename, this);
    load();
    connect(editor, &QPlainTextEdit::modificationChanged, this, &CodeSheet::setModified);

    // Initialize actions:

    addActionSeparator();

    auto actionFind = app->actions.getFind(this);
    addAction(actionFind);
    connect(actionFind, &QAction::triggered, this, &CodeSheet::findSelectedText);

    auto actionFindNext = app->actions.getFindNext(this);
    addAction(actionFindNext);
    connect(actionFindNext, &QAction::triggered, this, [=]() {
        searchBar->show();
        editor->nextSearchQuery();
    });

    auto actionFindPrevious = app->actions.getFindPrevious(this);
    addAction(actionFindPrevious);
    connect(actionFindPrevious, &QAction::triggered, this, [=]() {
        searchBar->show();
        editor->prevSearchQuery();
    });

    auto actionReplace = app->actions.getReplace(this);
    addAction(actionReplace);
    connect(actionReplace, &QAction::triggered, this, &CodeSheet::replaceSelectedText);
}

bool CodeSheet::load()
{
    file->close();
    if (file->open(QFile::ReadWrite)) {
        QTextStream stream(file);
        stream.setCodec("UTF-8");
        auto cursor = editor->textCursor();
        const int selectionStart = cursor.selectionStart();
        const int selectionEnd = cursor.selectionEnd();
        const int scrollPosition = editor->verticalScrollBar()->value();
        editor->setPlainText(stream.readAll());
        codec = stream.codec();
        cursor.setPosition(selectionStart);
        cursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
        editor->setTextCursor(cursor);
        editor->verticalScrollBar()->setValue(scrollPosition);
        setModified(false);
        return true;
    } else {
        qWarning() << "Error: Could not open code resource file";
        return false;
    }
}

bool CodeSheet::save(const QString &as)
{
    QFile *file = this->file;
    if (!as.isEmpty()) {
        file = new QFile(as, this);
        file->open(QFile::WriteOnly);
    }

    bool result;
    if (file->isWritable()) {
        file->resize(0);
        QTextStream stream(file);
        stream.setCodec(codec);
        stream.setGenerateByteOrderMark(codec->name() != "UTF-8");
        stream << editor->toPlainText();
        if (as.isEmpty()) {
            setModified(false);
            emit saved();
        }
        result = true;
    } else {
        qWarning() << "Error: Could not save code resource file";
        result = false;
    }

    if (!as.isEmpty()) {
        delete file;
    }
    return result;
}

void CodeSheet::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        searchBar->hide();
    }
    BaseFileSheet::keyPressEvent(event);
}

void CodeSheet::findSelectedText()
{
    auto cursor = editor->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    const auto selection(cursor.selectedText());
    cursor.setPosition(cursor.selectionStart()); // Move to the beginning of the selection and clear it
    editor->setTextCursor(cursor);
    searchBar->setFindText(selection);
    searchBar->show();
    searchBar->focusFindBar();
}

void CodeSheet::replaceSelectedText()
{
    const auto selection = editor->textCursor().selectedText();
    searchBar->show();
    searchBar->showReplaceBar();
    if (selection.isEmpty()) {
        searchBar->focusFindBar();
    } else {
        searchBar->setFindText(selection);
        searchBar->focusReplaceBar();
    }
}
