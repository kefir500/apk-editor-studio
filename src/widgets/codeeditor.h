#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <KSyntaxHighlighting/Theme>
#include <QPlainTextEdit>

namespace KSyntaxHighlighting {
    class SyntaxHighlighter;
    class Definition;
}

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    friend class CodeSideBar;

public:
    CodeEditor(QWidget *parent = nullptr);

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
    CodeSideBar *sidebar;
    KSyntaxHighlighting::SyntaxHighlighter *highlighter;
};

#endif // CODEEDITOR_H
