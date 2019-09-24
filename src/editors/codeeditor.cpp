#include "editors/codeeditor.h"
#include "base/application.h"
#include "base/fileformatlist.h"
#include "base/yamlhighlighter.h"
#include "base/xmlhighlighter.h"
#include <QBoxLayout>
#include <QTextStream>
#include <QScrollBar>
#include <QPainter>
#include <QDebug>

// CodeEditor:

CodeEditor::CodeEditor(const ResourceModelIndex &index, QWidget *parent) : FileEditor(index, parent)
{
    const QString filename = index.path();
    title = filename.section('/', -2);
    QRegularExpression guid("^{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}}");
    if (guid.match(title).hasMatch()) {
        title = title.split('/').last();
    }
    icon = index.icon();

    editor = new CodeTextEdit(this);
    new CodeContainer(editor);

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

    connect(editor, &QPlainTextEdit::modificationChanged, [=](bool changed) {
        setModified(changed);
    });
}

bool CodeEditor::load()
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

// CodeTextEdit:

CodeTextEdit::CodeTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
    setLineWrapMode(CodeTextEdit::NoWrap);

    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
#ifndef Q_OS_OSX
    font.setPointSize(10);
#else
    font.setPointSize(12);
#endif
    setFont(font);
    setTabStopWidth(4 * QFontMetrics(font).width(' '));
}

void CodeTextEdit::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    emit resized();
}

// CodeContainer:

CodeContainer::CodeContainer(CodeTextEdit *editor) : QWidget(editor)
{
    m_width = 0;
    m_padding = 20;
    connect(editor, &CodeTextEdit::blockCountChanged, [=](int blocks) {
        m_width = editor->fontMetrics().boundingRect(QString::number(blocks)).width() + m_padding;
        editor->setViewportMargins(m_width, 0, 0, 0);
    });
    connect(editor, &CodeTextEdit::resized, [=]() {
        const QRect contents = editor->contentsRect();
        setGeometry(QRect(contents.left(), contents.top(), m_width, contents.height()));
    });
    connect(editor, &CodeTextEdit::cursorPositionChanged, [=]() {
        QTextEdit::ExtraSelection selection;
        QColor highlight = QPalette().color(QPalette::Highlight);
        highlight.setAlpha(16);
        selection.format.setBackground(highlight);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = editor->textCursor();
        selection.cursor.clearSelection();
        editor->setExtraSelections({selection});
        m_currentLine = editor->textCursor().block().blockNumber() + 1;
    });
    connect(editor, &CodeTextEdit::updateRequest, [=](const QRect &rect, int dy) {
        dy ? scroll(0, dy) : update(0, rect.y(), width(), rect.height());
    });
}

CodeTextEdit *CodeContainer::parent() const
{
    return qobject_cast<CodeTextEdit *>(QWidget::parent());
}

QSize CodeContainer::sizeHint() const
{
    return QSize(m_width, parent()->viewport()->height());
}

void CodeContainer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, m_width, height(), QColor::fromRgb(245, 245, 245));
    painter.setPen(QColor::fromRgb(140, 140, 140));
    const int lineNumberHeight = parent()->fontMetrics().height();
    QTextBlock block = parent()->firstVisibleBlock();
    int top = static_cast<int>(parent()->blockBoundingGeometry(block).translated(parent()->contentOffset()).top());
    int bottom = top + static_cast<int>(parent()->blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const int blockNumber = block.blockNumber() + 1;
            if (blockNumber != m_currentLine) {
                painter.drawText(0, top, m_width - m_padding / 2, lineNumberHeight, Qt::AlignRight, QString::number(blockNumber));
            } else {
                painter.save();
                QFont font = painter.font();
                font.setBold(true);
                painter.setPen(QColor::fromRgb(100, 100, 100));
                painter.setFont(font);
                painter.drawText(0, top, m_width - m_padding / 2, lineNumberHeight, Qt::AlignRight, QString::number(blockNumber));
                painter.restore();
            }
        }
        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(parent()->blockBoundingRect(block).height());
    }
}
