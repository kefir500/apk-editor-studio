#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

class QCheckBox;
class QTableView;
class UpdateItemsModel;

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = nullptr);

    void checkUpdates();

protected:
    void changeEvent(QEvent *event);

private:
    void refreshUpdateButton(int row);
    void refreshWhatsNewButton(int row);
    void retranslate();

    UpdateItemsModel *updatesModel;
    QTableView *table;
    QPushButton *btnRefresh;
    QCheckBox *checkboxAutoUpdate;
};

#endif // UPDATEDIALOG_H
