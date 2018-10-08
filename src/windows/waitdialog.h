#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QMessageBox>

class WaitDialog : public QMessageBox
{
    Q_OBJECT

public:
    explicit WaitDialog(QWidget *parent = nullptr);
    ~WaitDialog();
};

#define WAIT WaitDialog waitDialog(this); Q_UNUSED(waitDialog)

#endif // WAITDIALOG_H
