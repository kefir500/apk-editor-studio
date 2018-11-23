#ifndef TITLEEDITOR_H
#define TITLEEDITOR_H

#include "editors/editor.h"
#include "apk/titlesmodel.h"
#include <QTableView>

class TitleEditor : public Editor
{
    Q_OBJECT

public:
    TitleEditor(const Project *project, QWidget *parent = nullptr);
    bool save(const QString &as = QString()) Q_DECL_OVERRIDE;

private:
    QTableView *table;
    TitlesModel *model;
};

#endif // TITLEEDITOR_H
