#ifndef KEYCREATOR_H
#define KEYCREATOR_H

#include "widgets/loadingwidget.h"
#include "tools/keystore.h"
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>

class KeyCreator : public QDialog
{
    Q_OBJECT

public:
    KeyCreator(const QString &keystorePath, const QString &keystorePassword, QWidget *parent = nullptr);

signals:
    void createdKey(const QString &alias);

protected:
    virtual void create();
    virtual bool validate();
    void fillKeystore(Keystore &keystore, const QString &keystorePath, const QString &keystorePassword) const;

    const QString strPasswordMinimum{tr("Password must be at least 6 characters.")};
    const QString strPasswordMatch{tr("Passwords do not match.")};
    const QString strFieldRequired{tr("Please fill out the required fields.")};

    QFormLayout *mainLayout;
    LoadingWidget *loading;

private:
    const QString keystorePath;
    const QString keystorePassword;

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

#endif // KEYCREATOR_H
