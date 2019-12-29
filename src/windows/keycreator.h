#ifndef KEYCREATOR_H
#define KEYCREATOR_H

#include "widgets/loadingwidget.h"
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>

class KeyCreator : public QDialog
{
    Q_OBJECT

public:

    explicit KeyCreator(QWidget *parent = nullptr);
    KeyCreator(const QString &keystorePath, const QString &keystorePassword, QWidget *parent = nullptr);

signals:
    void createdKeystore(const QString &path);
    void createdKey(const QString &alias);

private:
    enum class Type {
        Keystore,
        Key
    };

    QFormLayout *initialize();
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

#endif // KEYCREATOR_H
