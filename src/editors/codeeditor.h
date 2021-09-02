#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "editors/fileeditor.h"
#include <QTextCodec>
#include <QPlainTextEdit>

namespace KSyntaxHighlighting {
    class SyntaxHighlighter;
    class Definition;
    class Theme;
}

class CodeTextEdit;

class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    LineNumberArea(CodeTextEdit *parent);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

protected:
    void changeEvent(QEvent *event) override;

private:
    void updateAreaWidth(int blocks);
    void updateAreaGeometry();

    CodeTextEdit *editor;
    int lineNumberAreaWidth;
    int lineNumberAreaPadding;
    int currentLineNumber;
};

class CodeTextEdit : public QPlainTextEdit
{
    Q_OBJECT
    friend LineNumberArea;

public:
    CodeTextEdit(QWidget *parent = nullptr);

    const KSyntaxHighlighting::Theme getTheme() const;
    int getTabWidth() const;

    void setTheme(const KSyntaxHighlighting::Theme &theme);
    void setDefinition(const KSyntaxHighlighting::Definition &definition);

signals:
    void resized();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    KSyntaxHighlighting::SyntaxHighlighter *highlighter;
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
