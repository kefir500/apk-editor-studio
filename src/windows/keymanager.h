#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "widgets/filebox.h"
#include <QDialog>
#include <QGroupBox>

class KeyManager : public QDialog
{
    Q_OBJECT

public:
    explicit KeyManager(QWidget *parent = nullptr);

    void load();
    void save();

    void createKeystore();
    void createKey();

private:
    QGroupBox *groupKeystore;
    FileBox *editKeystore;
    QLineEdit *editKeystorePassword;
    QLineEdit *editKeyAlias;
    QLineEdit *editKeyPassword;
    QPushButton *btnCreateKeystore;
    QPushButton *btnCreateKey;
    QToolButton *btnSelectKey;
};

#endif // KEYMANAGER_H
