#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "editors/baseeditor.h"
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>

class CodeEditor : public BaseEditor
{
public:
    CodeEditor(const QString &filename, const QPixmap &icon = QPixmap(), QWidget *parent = nullptr);
    bool load() Q_DECL_OVERRIDE;
    bool save(const QString &as = QString()) Q_DECL_OVERRIDE;

private:
    QFile *file;
    QPlainTextEdit *editor;
    QSyntaxHighlighter *syntax;
};

#endif // CODEEDITOR_H
