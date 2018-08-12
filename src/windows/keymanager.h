#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#include "widgets/filebox.h"
#include "base/result.h"
#include <QDialog>
#include <QGroupBox>
#include <QSpinBox>
#include <QFormLayout>

// KeyManager

class KeyManager : public QDialog
{
    Q_OBJECT

public:
    explicit KeyManager(QWidget *parent = 0);

    void load();
    void save();

    bool createKeystore();
    bool createKey();

    static QString selectKey(const QString &keystore, const QString &password, const QString &currentAlias = QString(), QWidget *parent = 0);
    static Result<QStringList> getCertificates(const QString &keystore, const QString &password, QWidget *parent = 0);

private:
//    QStringList fetchCertificates();
    QStringList parseCertificates(const QString &output);

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

    explicit KeyCreator(QWidget *parent = 0);
    KeyCreator(const QString &keystorePath, const QString &keystorePassword, QWidget *parent = 0);

signals:
    void createdKeystore(const QString &path);
    void createdKey(const QString &alias);

private:
    QFormLayout *initialize(Type type);
    QFormLayout *createKeyLayout();
    bool validateFields();
    bool create();

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
};

#endif // KEYMANAGER_H
