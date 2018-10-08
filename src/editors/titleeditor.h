#ifndef TITLEEDITOR_H
#define TITLEEDITOR_H

#include "editors/baseeditor.h"
#include "apk/titlesmodel.h"
#include <QTableView>

class TitleEditor : public BaseEditor
{
    Q_OBJECT

public:
    TitleEditor(const Project *project, QWidget *parent = nullptr);
    bool load() Q_DECL_OVERRIDE;
    bool save(const QString &as = QString()) Q_DECL_OVERRIDE;

private:
    QTableView *table;
    TitlesModel *model;
};

#endif // TITLEEDITOR_H
