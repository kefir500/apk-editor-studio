#include "sheets/codesheet.h"
#include "widgets/codeeditor.h"
#include "base/application.h"
#include <KSyntaxHighlighting/Definition>
#include <QBoxLayout>
#include <QDebug>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextCodec>
#include <QTextStream>

CodeSheet::CodeSheet(const ResourceModelIndex &index, QWidget *parent) : BaseFileSheet(index, parent)
{
    const QString filename = index.path();
    title = filename.section('/', -2);
    QRegularExpression guid("^{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}}");
    if (guid.match(title).hasMatch()) {
        title = title.split('/').last();
    }
    icon = index.icon();

    editor = new CodeEditor(this);
    editor->setDefinition(app->highlightingRepository.definitionForFileName(filename));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    layout->setMargin(0);

    file = new QFile(filename, this);
    load();

    connect(editor, &QPlainTextEdit::modificationChanged, this, &CodeSheet::setModified);
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








