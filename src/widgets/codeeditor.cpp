#include "widgets/codeeditor.h"
#include "widgets/codesidebar.h"
#include "base/application.h"
#include "base/utils.h"
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <QRegularExpression>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , sidebar(new CodeSideBar(this))
    , highlighter(new KSyntaxHighlighting::SyntaxHighlighter(document()))
{
    setLineWrapMode(CodeEditor::NoWrap);

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

int CodeEditor::getTabWidth() const
{
    if (highlighter->definition().name() == "YAML") {
        return 2;
    }
    return 4;
}

QRgb CodeEditor::getColor(KSyntaxHighlighting::Theme::EditorColorRole role) const
{
    return highlighter->theme().editorColor(role);
}

QTextBlock CodeEditor::blockAtPosition(int y) const
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

bool CodeEditor::isFoldable(const QTextBlock &block) const
{
    return highlighter->startsFoldingRegion(block);
}

bool CodeEditor::isFolded(const QTextBlock &block) const
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

void CodeEditor::toggleFold(const QTextBlock &startBlock)
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

void CodeEditor::setTheme(const KSyntaxHighlighting::Theme &theme)
{
    auto newPalette(palette());
    newPalette.setColor(QPalette::Base, theme.editorColor(KSyntaxHighlighting::Theme::BackgroundColor));
    newPalette.setColor(QPalette::Highlight, theme.editorColor(KSyntaxHighlighting::Theme::TextSelection));
    setPalette(newPalette);
    highlighter->setTheme(theme);
    highlighter->rehighlight();
}

void CodeEditor::setDefinition(const KSyntaxHighlighting::Definition &definition)
{
    highlighter->setDefinition(definition);
    highlighter->rehighlight();
    setTabStopDistance(getTabWidth() * QFontMetrics(font()).horizontalAdvance(' '));
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    sidebar->updateSidebarGeometry();
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
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
