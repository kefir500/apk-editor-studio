#include "editors/codeeditor.h"
#include "base/application.h"
#include "base/fileformatlist.h"
#include "base/yamlhighlighter.h"
#include "base/xmlhighlighter.h"
#include <QBoxLayout>
#include <QTextStream>
#include <QDebug>

CodeEditor::CodeEditor(const ResourceModelIndex &index, QWidget *parent) : FileEditor(index, parent)
{
    const QString filename = index.path();
    title = filename.section('/', -2);
    icon = index.icon();

    editor = new QPlainTextEdit(this);
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
#ifndef Q_OS_OSX
    font.setPointSize(10);
#else
    font.setPointSize(12);
#endif
    editor->setFont(font);
    editor->setTabStopWidth(4 * QFontMetrics(font).width(' '));

    const QString suffix = QFileInfo(filename).suffix().toLower();
    if (FileFormat::fromExtension("xml").hasExtension(suffix) || FileFormat::fromExtension("html").hasExtension(suffix)) {
        syntax = new XmlHighlighter(editor->document());
    } else if (FileFormat::fromExtension("yml").getExtensions().contains(suffix)) {
        syntax = new YamlHighlighter(editor->document());
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    layout->setMargin(0);

    file = new QFile(filename, this);
    load();

    connect(editor, &QPlainTextEdit::textChanged, [=]() {
        setModified(true);
    });
}

bool CodeEditor::load()
{
    file->close();
    if (file->open(QFile::ReadWrite)) {
        QTextStream stream(file);
        stream.setCodec("UTF-8");
        editor->setPlainText(stream.readAll());
        setModified(false);
        return true;
    } else {
        qWarning() << "Error: Could not open code resource file";
        return false;
    }
}

bool CodeEditor::save(const QString &as)
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
        stream.setCodec("UTF-8");
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

QStringList CodeEditor::supportedFormats()
{
    FileFormatList filter;
    filter.add(FileFormat::fromExtension("xml"));
    filter.add(FileFormat::fromExtension("html"));
    filter.add(FileFormat::fromExtension("yml"));
    return filter.getExtensions();
}
