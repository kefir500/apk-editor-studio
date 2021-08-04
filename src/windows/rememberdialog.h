#ifndef REMEMBERDIALOG_H
#define REMEMBERDIALOG_H

#include <QCoreApplication>

class RememberDialog
{
    Q_DECLARE_TR_FUNCTIONS(RememberDialog)

public:
    static void say(const QString &identifier, const QString &message, QWidget *parent = nullptr);
    static bool ask(const QString &identifier, const QString &question, QWidget *parent = nullptr);
};

#endif // REMEMBERDIALOG_H
