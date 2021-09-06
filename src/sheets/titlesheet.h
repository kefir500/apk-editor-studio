#ifndef TITLEEDITOR_H
#define TITLEEDITOR_H

#include "sheets/baseeditablesheet.h"
#include "apk/titleitemsmodel.h"
#include <QTableView>

class TitleSheet : public BaseEditableSheet
{
    Q_OBJECT

public:
    TitleSheet(const Project *project, QWidget *parent = nullptr);
    bool save(const QString &as = QString()) override;

private:
    QTableView *table;
    TitleItemsModel *model = nullptr;
};

#endif // TITLEEDITOR_H
