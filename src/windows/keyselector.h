#ifndef KEYSELECTOR_H
#define KEYSELECTOR_H

#include "widgets/loadingwidget.h"
#include <QDialog>
#include <QComboBox>

class KeySelector : public QDialog
{
    Q_OBJECT

public:
    KeySelector(const QString &keystore, const QString &password,
                const QString &defaultAlias = QString(), QWidget *parent = nullptr);

    QString select();
    QString getCurrentValue() const;

private:
    void refresh(const QString &defaultAlias);

    const QString keystore;
    const QString password;

    QComboBox *combo;
    LoadingWidget *loading;
};

#endif // KEYSELECTOR_H
