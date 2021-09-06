#include "widgets/codesidebar.h"
#include "widgets/codeeditor.h"
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <QPainter>
#include <QPainterPath>

CodeSideBar::CodeSideBar(CodeEditor *parent)
    : QWidget(parent)
    , editor(parent)
{
    connect(editor, &CodeEditor::blockCountChanged, this, &CodeSideBar::updateSidebarWidth);
    connect(editor, &CodeEditor::updateRequest, this, [=](const QRect &rect, int dy) {
        dy ? scroll(0, dy) : update(0, rect.y(), width(), rect.height());
    });
}

void CodeSideBar::setCurrentLine(int line)
{
    currentLineNumber = line;
}

void CodeSideBar::updateSidebarWidth(int blocks)
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

QSize CodeSideBar::sizeHint() const
{
    return QSize(sidebarWidth, editor->viewport()->height());
}

void CodeSideBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LayoutDirectionChange) {
        updateSidebarWidth(editor->blockCount());
        updateSidebarGeometry();
    }
    QWidget::changeEvent(event);
}

void CodeSideBar::mouseReleaseEvent(QMouseEvent *event)
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

void CodeSideBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), editor->getEditorColor(KSyntaxHighlighting::Theme::IconBorder));

    QTextBlock block = editor->firstVisibleBlock();
    int top = static_cast<int>(editor->blockBoundingGeometry(block).translated(editor->contentOffset()).top());
    int bottom = top + static_cast<int>(editor->blockBoundingRect(block).height());
    const int lineNumberHeight = editor->fontMetrics().height();
    const int foldingMarkerSize = editor->fontMetrics().lineSpacing();

    while (block.isValid() && top <= event->rect().bottom()) {

        // Line number
        if (block.isVisible() && bottom >= event->rect().top()) {
            const int blockNumber = block.blockNumber() + 1;
            painter.setPen(editor->getEditorColor(blockNumber == currentLineNumber
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
            painter.setPen(QPen(QColor(editor->getEditorColor(KSyntaxHighlighting::Theme::LineNumbers)), 2));
            painter.translate(layoutDirection() == Qt::LeftToRight ? width() - foldingMarkerSize : 0, top);
            painter.drawPath(foldingMarker);
            painter.restore();
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(editor->blockBoundingRect(block).height());
    }
}

void CodeSideBar::updateSidebarGeometry()
{
    const QRect contents = editor->contentsRect();
    if (layoutDirection() == Qt::LeftToRight) {
        setGeometry(QRect(contents.left(), contents.top(), sidebarWidth, contents.height()));
    } else {
        setGeometry(QRect(contents.right() - sidebarWidth, contents.top(), sidebarWidth, contents.height()));
    }
}
