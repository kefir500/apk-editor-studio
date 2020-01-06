#include "windows/keycreator.h"
#include "windows/dialogs.h"
#include "tools/keytool.h"
#include <QGroupBox>
#include <QDialogButtonBox>

KeyCreator::KeyCreator(const QString &keystorePath, const QString &keystorePassword, QWidget *parent)
    : QDialog(parent)
    , keystorePath(keystorePath)
    , keystorePassword(keystorePassword)
{
    setWindowTitle(tr("Create Key"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    mainLayout = new QFormLayout(this);

    QFormLayout *keyLayout = new QFormLayout;
    keyLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    editAlias = new QLineEdit(this);
    editKeyPassword = new QLineEdit(this);
    editKeyPasswordConfirm = new QLineEdit(this);
    editYears = new QSpinBox(this);
    editName = new QLineEdit(this);
    editUnit = new QLineEdit(this);
    editOrganization = new QLineEdit(this);
    editCity = new QLineEdit(this);
    editState = new QLineEdit(this);
    editCountry = new QLineEdit(this);

    editAlias->setPlaceholderText(tr("Alias"));
    editKeyPassword->setPlaceholderText(tr("Key Password"));
    editKeyPasswordConfirm->setPlaceholderText(tr("Confirm Password"));
    editName->setPlaceholderText(tr("First and Last Name"));
    editUnit->setPlaceholderText(tr("Organizational Unit"));
    editOrganization->setPlaceholderText(tr("Organization"));
    editCity->setPlaceholderText(tr("City or Locality"));
    editState->setPlaceholderText(tr("State or Province"));
    editCountry->setPlaceholderText(tr("Country Code"));

    editKeyPassword->setEchoMode(QLineEdit::Password);
    editKeyPasswordConfirm->setEchoMode(QLineEdit::Password);
    editYears->setRange(1, 999999);
    editYears->setValue(25);

    QHBoxLayout *layoutPassword = new QHBoxLayout;
    layoutPassword->addWidget(editKeyPassword);
    layoutPassword->addWidget(editKeyPasswordConfirm);

    keyLayout->addRow(editAlias->placeholderText(), editAlias);
    keyLayout->addRow(tr("Password"), layoutPassword);
    keyLayout->addRow(tr("Validity (Years)"), editYears);
    keyLayout->addRow(editName->placeholderText(), editName);
    keyLayout->addRow(editUnit->placeholderText(), editUnit);
    keyLayout->addRow(editOrganization->placeholderText(), editOrganization);
    keyLayout->addRow(editCity->placeholderText(), editCity);
    keyLayout->addRow(editState->placeholderText(), editState);
    keyLayout->addRow(editCountry->placeholderText(), editCountry);

    auto groupKey = new QGroupBox(tr("Key"), this);
    groupKey->setLayout(keyLayout);
    mainLayout->addRow(groupKey);

    loading = new LoadingWidget(this);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &KeyCreator::create);
    connect(buttons, &QDialogButtonBox::rejected, this, &KeyCreator::reject);
    mainLayout->addRow(buttons);

    // Check if the KeyStore is valid

    if (!keystorePath.isNull()) {
        auto keytool = new Keytool::Aliases(keystorePath, keystorePassword, this);
        connect(keytool, &Keytool::Aliases::success, [=]() {
            loading->hide();
            keytool->deleteLater();
        });
        connect(keytool, &Keytool::Aliases::error, [=](Keytool::Aliases::ErrorType, const QString &brief, const QString &detailed) {
            Dialogs::detailed(brief, detailed, QMessageBox::Warning, this);
            keytool->deleteLater();
            close();
        });
        keytool->run();
    }
}

void KeyCreator::create()
{
    if (!validate()) {
        return;
    }

    Keystore keystore;
    fillKeystore(keystore, keystorePath, keystorePassword);

    loading->show();
    auto keytool = new Keytool::Genkey(keystore, this);
    connect(keytool, &Keytool::Genkey::success, [=]() {
        loading->hide();
        QMessageBox::information(this, QString(), tr("Key has been successfully created!"));
        emit createdKey(keystore.keyAlias);
        accept();
        keytool->deleteLater();
    });
    connect(keytool, &Keytool::Genkey::error, [=](Keytool::Genkey::ErrorType errorType, const QString &brief, const QString &detailed) {
        if (errorType == Keytool::Genkey::AliasExistsError) {
            editAlias->setFocus();
            editAlias->selectAll();
        }
        loading->hide();
        Dialogs::detailed(brief, detailed, QMessageBox::Warning, this);
        keytool->deleteLater();
    });
    keytool->run();
}

bool KeyCreator::validate()
{
    if (editAlias->text().isEmpty()) {
        QMessageBox::warning(this, QString(), strFieldRequired);
        editAlias->setFocus();
        return false;
    }
    if (editKeyPassword->text().length() < 6) {
        QMessageBox::warning(this, QString(), strPasswordMinimum);
        editKeyPassword->setFocus();
        return false;
    }
    if (editKeyPassword->text() != editKeyPasswordConfirm->text()) {
        QMessageBox::warning(this, QString(), strPasswordMatch);
        editKeyPasswordConfirm->setFocus();
        return false;
    }
    if (editName->text().isEmpty()) {
        QMessageBox::warning(this, QString(), strFieldRequired);
        editName->setFocus();
        return false;
    }
    return true;
}

void KeyCreator::fillKeystore(Keystore &keystore, const QString &keystorePath, const QString &keystorePassword) const
{
    keystore.keystorePath = keystorePath;
    keystore.keystorePassword = keystorePassword;
    keystore.keyAlias = editAlias->text();
    keystore.keyPassword = editKeyPassword->text();
    keystore.validity = editYears->text().toInt() * 365;
    keystore.dname.name = editName->text();
    keystore.dname.org = editOrganization->text();
    keystore.dname.orgUnit = editUnit->text();
    keystore.dname.city = editCity->text();
    keystore.dname.state = editState->text();
    keystore.dname.countryCode = editCountry->text();
}
