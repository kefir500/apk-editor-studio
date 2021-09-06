#ifndef CODESHEET_H
#define CODESHEET_H

#include "sheets/basefilesheet.h"

class CodeEditor;
class CodeSearchBar;
class QFile;

class CodeSheet : public BaseFileSheet
{
public:
    CodeSheet(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() override;
    bool save(const QString &as = QString()) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void findSelectedText();
    void replaceSelectedText();

    QFile *file;
    QTextCodec *codec;
    CodeEditor *editor;
    CodeSearchBar *searchBar;
};

#endif // CODESHEET_H
