#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "editors/fileeditor.h"
#include <KSyntaxHighlighting/Theme>
#include <QTextCodec>
#include <QPlainTextEdit>

namespace KSyntaxHighlighting {
    class SyntaxHighlighter;
    class Definition;
}

class CodeTextEdit : public QPlainTextEdit
{
    Q_OBJECT
    friend class CodeTextEditSidebar;

public:
    CodeTextEdit(QWidget *parent = nullptr);

    int getTabWidth() const;
    QRgb getColor(KSyntaxHighlighting::Theme::EditorColorRole) const;
    QTextBlock blockAtPosition(int y) const;
    bool isFoldable(const QTextBlock &block) const;
    bool isFolded(const QTextBlock &block) const;
    void toggleFold(const QTextBlock &startBlock);
    void setTheme(const KSyntaxHighlighting::Theme &theme);
    void setDefinition(const KSyntaxHighlighting::Definition &definition);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    CodeTextEditSidebar *sidebar;
    KSyntaxHighlighting::SyntaxHighlighter *highlighter;
};

class CodeTextEditSidebar : public QWidget
{
    Q_OBJECT
    friend class CodeTextEdit;

public:
    CodeTextEditSidebar(CodeTextEdit *parent);
    QSize sizeHint() const override;

protected:
    void changeEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void highlightCurrentLine();
    void updateSidebarWidth(int blocks);
    void updateSidebarGeometry();

    CodeTextEdit *editor;
    int sidebarWidth = 0;
    int sidebarPadding = 7;
    int currentLineNumber;
};

class CodeEditor : public FileEditor
{
public:
    CodeEditor(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() override;
    bool save(const QString &as = QString()) override;

private:
    QFile *file;
    QTextCodec *codec;
    CodeTextEdit *editor;
};

#endif // CODEEDITOR_H
