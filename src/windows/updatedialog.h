#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

class QTableView;
class UpdateItemsModel;

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = nullptr);

    void checkUpdates();

private:
    void refreshUpdateButton(int row);
    void refreshWhatsNewButton(int row);

    UpdateItemsModel *updatesModel;
    QTableView *table;
};

#endif // UPDATEDIALOG_H
