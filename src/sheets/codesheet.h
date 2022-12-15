#ifndef CODESHEET_H
#define CODESHEET_H

#include "sheets/basefilesheet.h"

class CodeEditor;
class CodeSearchBar;

class CodeSheet : public BaseFileSheet
{
    Q_OBJECT

public:
    CodeSheet(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() override;
    bool save(const QString &as = QString()) override;

    void setTextCursor(int lineNumber, int columnNumber, int selectionLength = 0);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void findSelectedText();
    void replaceSelectedText();

    QTextCodec *codec;
    CodeEditor *editor;
    CodeSearchBar *searchBar;
};

#endif // CODESHEET_H
