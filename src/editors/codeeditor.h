#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "editors/fileeditor.h"
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>

class CodeEditor : public FileEditor
{
public:
    CodeEditor(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() Q_DECL_OVERRIDE;
    bool save(const QString &as = QString()) Q_DECL_OVERRIDE;

    static QStringList supportedFormats();

private:
    QFile *file;
    QPlainTextEdit *editor;
    QSyntaxHighlighter *syntax;
};

#endif // CODEEDITOR_H
