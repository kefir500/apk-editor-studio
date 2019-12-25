#ifndef KEYSELECTOR_H
#define KEYSELECTOR_H

#include "widgets/loadingwidget.h"
#include <QDialog>
#include <QComboBox>

class KeySelector : public QDialog
{
public:
    KeySelector(QWidget *parent = nullptr);

    static QString select(const QString &keystore, const QString &password, const QString &currentAlias = QString(), QWidget *parent = nullptr);
    void refresh(const QString &keystore, const QString &password, const QString &currentAlias = QString());
    QString getCurrentValue() const;

private:
    QComboBox *combo;
    LoadingWidget *loading;
};

#endif // KEYSELECTOR_H
