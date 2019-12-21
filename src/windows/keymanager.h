#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "widgets/filebox.h"
#include "widgets/loadingwidget.h"
#include <QDialog>
#include <QGroupBox>
#include <QSpinBox>
#include <QFormLayout>

// KeyManager

class KeyManager : public QDialog
{
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

// KeyCreator

class KeyCreator : public QDialog
{
    Q_OBJECT

public:
    enum Type {
        TypeKeystore,
        TypeKey
    };

    explicit KeyCreator(QWidget *parent = nullptr);
    KeyCreator(const QString &keystorePath, const QString &keystorePassword, QWidget *parent = nullptr);

signals:
    void createdKeystore(const QString &path);
    void createdKey(const QString &alias);

private:
    QFormLayout *initialize(Type type);
    QFormLayout *createKeyLayout();
    bool validateFields();
    void create();

    Type type;
    QString keystorePath;
    QString keystorePassword;

    QLineEdit *editKeystorePassword;
    QLineEdit *editKeystorePasswordConfirm;

    QLineEdit *editAlias;
    QLineEdit *editKeyPassword;
    QLineEdit *editKeyPasswordConfirm;
    QSpinBox *editYears;
    QLineEdit *editName;
    QLineEdit *editUnit;
    QLineEdit *editOrganization;
    QLineEdit *editCity;
    QLineEdit *editState;
    QLineEdit *editCountry;
    LoadingWidget *loading;
};

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

#endif // KEYMANAGER_H
