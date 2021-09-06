#ifndef CODESHEET_H
#define CODESHEET_H

#include "sheets/basefilesheet.h"

class CodeEditor;
class QFile;
class QTextCodec;

class CodeSheet : public BaseFileSheet
{
public:
    CodeSheet(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() override;
    bool save(const QString &as = QString()) override;

private:
    QFile *file;
    QTextCodec *codec;
    CodeEditor *editor;
};

#endif // CODESHEET_H
