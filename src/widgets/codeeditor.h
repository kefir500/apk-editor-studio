#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <KSyntaxHighlighting/Theme>
#include <QPlainTextEdit>

class CodeSideBar;
namespace KSyntaxHighlighting {
    class SyntaxHighlighter;
    class Definition;
}

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    friend class CodeSideBar;

public:
    enum ExtraSelectionGroup {
        CurrentLineSelection,
        SearchResultSelection
    };

    CodeEditor(QWidget *parent = nullptr);

    int getTabWidth() const;
    QRgb getEditorColor(KSyntaxHighlighting::Theme::EditorColorRole) const;
    QRgb getTextColor(KSyntaxHighlighting::Theme::TextStyle) const;

    QTextBlock blockAtPosition(int y) const;
    bool isFoldable(const QTextBlock &block) const;
    bool isFolded(const QTextBlock &block) const;
    void toggleFold(const QTextBlock &startBlock);

    void replaceOne(const QString &with);
    void replaceAll(const QString &with);

    void setSearchQuery(const QString &query);
    void setSearchCaseSensitive(bool enabled);
    void nextSearchQuery(bool skipCurrent = true);
    void prevSearchQuery();

    void setTheme(const KSyntaxHighlighting::Theme &theme);
    void setDefinition(const KSyntaxHighlighting::Definition &definition);
    void setExtraSelectionGroup(ExtraSelectionGroup group, const QList<QTextEdit::ExtraSelection> &selection);

signals:
    void searchFinished(int totalResults, int currentResult = 0);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QTextCursor find(int from = 0, bool backward = false);
    QTextCursor find(const QTextCursor &cursor, bool backward = false);

    void highlightCurrentLine();
    void highlightSearchResults();

    CodeSideBar *sidebar;
    KSyntaxHighlighting::SyntaxHighlighter *highlighter;
    QMap<ExtraSelectionGroup, QList<QTextEdit::ExtraSelection>> extraSelections;
    QString searchQuery;
    bool searchCaseSensitive = false;
    QList<QTextCursor> searchResultCursors;
};

#endif // CODEEDITOR_H
