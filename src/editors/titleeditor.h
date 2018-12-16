#ifndef TITLEEDITOR_H
#define TITLEEDITOR_H

#include "editors/editor.h"
#include "apk/titleitemsmodel.h"
#include <QTableView>

class TitleEditor : public Editor
{
    Q_OBJECT

public:
    TitleEditor(const Project *project, QWidget *parent = nullptr);
    bool save(const QString &as = QString()) override;

private:
    QTableView *table;
    TitleItemsModel *model;
};

#endif // TITLEEDITOR_H
