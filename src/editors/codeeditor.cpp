#include "editors/codeeditor.h"
#include "base/application.h"
#include "base/utils.h"
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <QBoxLayout>
#include <QDebug>
#include <QFileInfo>
#include <QPainter>
#include <QPainterPath>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextStream>

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
    editor->setDefinition(app->highlightingRepository.definitionForFileName(filename));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    layout->setMargin(0);

    file = new QFile(filename, this);
    load();

    connect(editor, &QPlainTextEdit::modificationChanged, this, &CodeEditor::setModified);
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

// CodeTextEdit:

CodeTextEdit::CodeTextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
    , sidebar(new CodeTextEditSidebar(this))
    , highlighter(new KSyntaxHighlighting::SyntaxHighlighter(document()))
{
    setLineWrapMode(CodeTextEdit::NoWrap);

    const auto defaultTheme = app->highlightingRepository.defaultTheme(Utils::isDarkTheme()
        ? KSyntaxHighlighting::Repository::DarkTheme
        : KSyntaxHighlighting::Repository::LightTheme);
    setTheme(defaultTheme);

#if defined(Q_OS_WIN)
    QFont font("Consolas");
    font.setPointSize(11);
#elif defined(Q_OS_MACOS)
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(12);
#elif defined(Q_OS_LINUX)
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(10);
#endif
    setFont(font);
    sidebar->setFont(font);

}

int CodeTextEdit::getTabWidth() const
{
    if (highlighter->definition().name() == "YAML") {
        return 2;
    }
    return 4;
}

QRgb CodeTextEdit::getColor(KSyntaxHighlighting::Theme::EditorColorRole role) const
{
    return highlighter->theme().editorColor(role);
}

QTextBlock CodeTextEdit::blockAtPosition(int y) const
{
    auto block = firstVisibleBlock();
    if (!block.isValid()) {
        return QTextBlock();
    }

    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();
    do {
        if (top <= y && y <= bottom) {
            return block;
        }
        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
    } while (block.isValid());

    return QTextBlock();
}

bool CodeTextEdit::isFoldable(const QTextBlock &block) const
{
    return highlighter->startsFoldingRegion(block);
}

bool CodeTextEdit::isFolded(const QTextBlock &block) const
{
    if (!block.isValid()) {
        return false;
    }
    const auto nextBlock = block.next();
    if (!nextBlock.isValid()) {
        return false;
    }
    return !nextBlock.isVisible();
}

void CodeTextEdit::toggleFold(const QTextBlock &startBlock)
{
    const auto endBlock = highlighter->findFoldingRegionEnd(startBlock).next();
    if (isFolded(startBlock)) {
        auto block = startBlock.next();
        while (block.isValid() && !block.isVisible()) {
            block.setVisible(true);
            block.setLineCount(block.layout()->lineCount());
            block = block.next();
        }

    } else {
        auto block = startBlock.next();
        while (block.isValid() && block != endBlock) {
            block.setVisible(false);
            block.setLineCount(0);
            block = block.next();
        }
    }
    document()->markContentsDirty(startBlock.position(), endBlock.position() - startBlock.position() + 1);
    emit document()->documentLayout()->documentSizeChanged(document()->documentLayout()->documentSize());
}

void CodeTextEdit::setTheme(const KSyntaxHighlighting::Theme &theme)
{
    auto newPalette(palette());
    newPalette.setColor(QPalette::Base, theme.editorColor(KSyntaxHighlighting::Theme::BackgroundColor));
    newPalette.setColor(QPalette::Highlight, theme.editorColor(KSyntaxHighlighting::Theme::TextSelection));
    setPalette(newPalette);
    highlighter->setTheme(theme);
    highlighter->rehighlight();
}

void CodeTextEdit::setDefinition(const KSyntaxHighlighting::Definition &definition)
{
    highlighter->setDefinition(definition);
    highlighter->rehighlight();
    setTabStopDistance(getTabWidth() * QFontMetrics(font()).horizontalAdvance(' '));
}

void CodeTextEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    sidebar->updateSidebarGeometry();
}

void CodeTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab) {
        auto cursor = textCursor();
        cursor.setPosition(textCursor().selectionStart());
        const int startBlock = cursor.blockNumber();
        cursor.setPosition(textCursor().selectionEnd());
        const int endBlock = cursor.blockNumber();
        if (endBlock - startBlock > 0) {
            cursor.beginEditBlock();
            forever {
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.insertText(QString(' ').repeated(getTabWidth()));
                if (cursor.blockNumber() == startBlock) {
                    break;
                }
                cursor.movePosition(QTextCursor::PreviousBlock);
            }
            cursor.endEditBlock();
            return;
        }
    } else if (event->key() == Qt::Key_Backtab) {
        auto cursor = textCursor();
        cursor.setPosition(textCursor().selectionStart());
        const int startBlock = cursor.blockNumber();
        cursor.setPosition(textCursor().selectionEnd());
        cursor.beginEditBlock();
        forever {
            const auto indentRegex = QRegularExpression(QString("^( {1,%1}|\\t)").arg(getTabWidth()));
            const auto indentMatch = indentRegex.match(cursor.block().text());
            const int indentLength = indentMatch.capturedLength();
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, indentLength);
            cursor.removeSelectedText();
            if (cursor.blockNumber() == startBlock) {
                break;
            }
            cursor.movePosition(QTextCursor::PreviousBlock);
        }
        cursor.endEditBlock();
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
}

// CodeTextEditSidebar:

CodeTextEditSidebar::CodeTextEditSidebar(CodeTextEdit *parent)
    : QWidget(parent)
    , editor(parent)
{
    connect(editor, &CodeTextEdit::blockCountChanged, this, &CodeTextEditSidebar::updateSidebarWidth);
    connect(editor, &CodeTextEdit::cursorPositionChanged, this, &CodeTextEditSidebar::highlightCurrentLine);
    connect(editor, &CodeTextEdit::updateRequest, this, [=](const QRect &rect, int dy) {
        dy ? scroll(0, dy) : update(0, rect.y(), width(), rect.height());
    });
}

QSize CodeTextEditSidebar::sizeHint() const
{
    return QSize(sidebarWidth, editor->viewport()->height());
}

void CodeTextEditSidebar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LayoutDirectionChange) {
        updateSidebarWidth(editor->blockCount());
        updateSidebarGeometry();
    }
    QWidget::changeEvent(event);
}

void CodeTextEditSidebar::mouseReleaseEvent(QMouseEvent *event)
{
    const bool isFoldingMarkerClicked = layoutDirection() == Qt::LeftToRight
        ? event->x() >= width() - editor->fontMetrics().lineSpacing()
        : event->x() <= editor->fontMetrics().lineSpacing();

    if (isFoldingMarkerClicked) {
        auto block = editor->blockAtPosition(event->y());
        if (block.isValid() && editor->isFoldable(block)) {
            editor->toggleFold(block);
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void CodeTextEditSidebar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), editor->getColor(KSyntaxHighlighting::Theme::IconBorder));

    QTextBlock block = editor->firstVisibleBlock();
    int top = static_cast<int>(editor->blockBoundingGeometry(block).translated(editor->contentOffset()).top());
    int bottom = top + static_cast<int>(editor->blockBoundingRect(block).height());
    const int lineNumberHeight = editor->fontMetrics().height();
    const int foldingMarkerSize = editor->fontMetrics().lineSpacing();

    while (block.isValid() && top <= event->rect().bottom()) {

        // Line number
        if (block.isVisible() && bottom >= event->rect().top()) {
            const int blockNumber = block.blockNumber() + 1;
            painter.setPen(editor->getColor(blockNumber == currentLineNumber
                 ? KSyntaxHighlighting::Theme::CurrentLineNumber
                 : KSyntaxHighlighting::Theme::LineNumbers));
            painter.drawText(layoutDirection() == Qt::LeftToRight ? sidebarPadding : sidebarPadding + foldingMarkerSize,
                             top, width() - sidebarPadding * 2 - foldingMarkerSize, lineNumberHeight,
                             Qt::AlignRight, QString::number(blockNumber));
        }

        // Folding marker
        if (block.isVisible() && editor->highlighter->startsFoldingRegion(block)) {
            QPainterPath foldingMarker;
            if (!editor->isFolded(block)) {
                foldingMarker.moveTo(5, 7);
                foldingMarker.lineTo(foldingMarkerSize / 2, foldingMarkerSize - 7);
                foldingMarker.lineTo(foldingMarkerSize - 5, 7);
            } else if (layoutDirection() == Qt::LeftToRight) {
                foldingMarker.moveTo(7, 5);
                foldingMarker.lineTo(foldingMarkerSize - 7, foldingMarkerSize / 2);
                foldingMarker.lineTo(7, foldingMarkerSize - 5);
            } else if (layoutDirection() == Qt::RightToLeft) {
                foldingMarker.moveTo(foldingMarkerSize - 7, 5);
                foldingMarker.lineTo(7, foldingMarkerSize / 2);
                foldingMarker.lineTo(foldingMarkerSize - 7, foldingMarkerSize - 5);
            }
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(QPen(QColor(editor->getColor(KSyntaxHighlighting::Theme::LineNumbers)), 2));
            painter.translate(layoutDirection() == Qt::LeftToRight ? width() - foldingMarkerSize : 0, top);
            painter.drawPath(foldingMarker);
            painter.restore();
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(editor->blockBoundingRect(block).height());
    }
}

void CodeTextEditSidebar::highlightCurrentLine()
{
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(editor->getColor(KSyntaxHighlighting::Theme::CurrentLine)));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = editor->textCursor();
    selection.cursor.clearSelection();
    editor->setExtraSelections({selection});
    currentLineNumber = editor->textCursor().blockNumber() + 1;
}

void CodeTextEditSidebar::updateSidebarWidth(int blocks)
{
    sidebarWidth = fontMetrics().horizontalAdvance(QString::number(blocks))
        + fontMetrics().lineSpacing()
        + sidebarPadding * 2;
    if (layoutDirection() == Qt::LeftToRight) {
        editor->setViewportMargins(sidebarWidth, 0, 0, 0);
    } else {
        editor->setViewportMargins(0, 0, sidebarWidth, 0);
    }
}

void CodeTextEditSidebar::updateSidebarGeometry()
{
    const QRect contents = editor->contentsRect();
    if (layoutDirection() == Qt::LeftToRight) {
        setGeometry(QRect(contents.left(), contents.top(), sidebarWidth, contents.height()));
    } else {
        setGeometry(QRect(contents.right() - sidebarWidth, contents.top(), sidebarWidth, contents.height()));
    }
}
