#ifndef YESALWAYSDIALOG_H
#define YESALWAYSDIALOG_H

#include <QDialog>

class YesAlwaysDialog : public QDialog
{
    Q_OBJECT

public:
    static bool ask(const QString &identifier, const QString &question, QWidget *parent = nullptr);

protected:
    YesAlwaysDialog(const QString &identifier, const QString &question, QWidget *parent = nullptr);
};

#endif // YESALWAYSDIALOG_H
