#ifndef TITLEEDITOR_H
#define TITLEEDITOR_H

#include "sheets/baseeditablesheet.h"

class QTableView;
class Package;
class TitleItemsModel;

class TitleSheet : public BaseEditableSheet
{
    Q_OBJECT

public:
    TitleSheet(const Package *package, QWidget *parent = nullptr);
    bool save(const QString &as = QString()) override;

protected:
    void changeEvent(QEvent *event) override;

private:
    void retranslate();

    QTableView *table;
    TitleItemsModel *model = nullptr;
};

#endif // TITLEEDITOR_H
