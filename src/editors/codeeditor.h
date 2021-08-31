#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "editors/fileeditor.h"
#include <QTextCodec>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>

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

signals:
    void resized();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

class CodeEditor : public FileEditor
{
public:
    CodeEditor(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() override;
    bool save(const QString &as = QString()) override;

    static QStringList supportedFormats();

private:
    QFile *file;
    QTextCodec *codec;
    CodeTextEdit *editor;
    QSyntaxHighlighter *syntax;
};

#endif // CODEEDITOR_H
