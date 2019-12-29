#include "windows/keycreator.h"
#include "windows/dialogs.h"
#include "tools/keytool.h"
#include <QGroupBox>
#include <QDialogButtonBox>

KeyCreator::KeyCreator(QWidget *parent)
    : QDialog(parent)
    , type(Type::Keystore)
{
    initialize();
}

KeyCreator::KeyCreator(const QString &keystorePath, const QString &keystorePassword, QWidget *parent)
    : QDialog(parent)
    , type(Type::Key)
    , keystorePath(keystorePath)
    , keystorePassword(keystorePassword)
{
    initialize();
}

QFormLayout *KeyCreator::initialize()
{
    if (type == Type::Keystore) {
        setWindowTitle(tr("Create Keystore"));
    } else if (type == Type::Key) {
        setWindowTitle(tr("Create Key"));
    }
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QFormLayout *layout = new QFormLayout(this);
    QFormLayout *keyLayout = createKeyLayout();
    loading = new LoadingWidget(this);

    if (type == Type::Keystore) {
        editKeystorePassword = new QLineEdit(this);
        editKeystorePasswordConfirm = new QLineEdit(this);
        editKeystorePassword->setPlaceholderText(tr("Keystore Password"));
        editKeystorePasswordConfirm->setPlaceholderText(tr("Confirm Password"));
        editKeystorePassword->setEchoMode(QLineEdit::Password);
        editKeystorePasswordConfirm->setEchoMode(QLineEdit::Password);

        loading->hide();

        QHBoxLayout *layoutPassword = new QHBoxLayout;
        layoutPassword->addWidget(editKeystorePassword);
        layoutPassword->addWidget(editKeystorePasswordConfirm);
        layout->addRow(tr("Password"), layoutPassword);

        QGroupBox *groupKey = new QGroupBox(tr("Key"), this);
        groupKey->setLayout(keyLayout);
        layout->addRow(groupKey);

    } else if (type == Type::Key) {
        editKeystorePassword = nullptr;
        editKeystorePasswordConfirm = nullptr;
        layout->addRow(keyLayout);

        // Check if the KeyStore is valid

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

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &KeyCreator::create);
    connect(buttons, &QDialogButtonBox::rejected, this, &KeyCreator::reject);
    layout->addRow(buttons);

    return layout;
}

void KeyCreator::create()
{
    if (!validateFields()) {
        return;
    }

    QString path;
    if (type == Type::Keystore) {
        path = Dialogs::getSaveKeystoreFilename();
        if (path.isEmpty()) {
            return;
        }
        QFile::remove(path);
    } else if (type == Type::Key) {
        path = keystorePath;
    }

    Keystore keystore;
    keystore.keystorePath = path;
    keystore.keystorePassword = keystorePassword.isEmpty() ? editKeystorePassword->text() : keystorePassword;
    keystore.keyAlias = editAlias->text();
    keystore.keyPassword = editKeyPassword->text();
    keystore.validity = editYears->text().toInt() * 365;
    keystore.dname.name = editName->text();
    keystore.dname.org = editOrganization->text();
    keystore.dname.orgUnit = editUnit->text();
    keystore.dname.city = editCity->text();
    keystore.dname.state = editState->text();
    keystore.dname.countryCode = editCountry->text();

    loading->show();
    auto keytool = new Keytool::Genkey(keystore, this);
    connect(keytool, &Keytool::Genkey::success, [=]() {
        loading->hide();
        if (type == Type::Keystore) {
            QMessageBox::information(this, QString(), tr("Keystore has been successfully created!"));
            emit createdKeystore(keystore.keystorePath);
            emit createdKey(keystore.keyAlias);
        } else if (type == Type::Key) {
            QMessageBox::information(this, QString(), tr("Key has been successfully created!"));
            emit createdKey(keystore.keyAlias);
        }
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

bool KeyCreator::validateFields()
{
    const QString strPasswordMinimum = tr("Password must be at least 6 characters.");
    const QString strPasswordMatch = tr("Passwords do not match.");
    const QString strFieldRequired = tr("Please fill out the required fields.");

    if (editKeystorePassword && editKeystorePassword->text().length() < 6) {
        QMessageBox::warning(this, QString(), strPasswordMinimum);
        editKeystorePassword->setFocus();
        return false;
    }
    if (editKeystorePassword && editKeystorePassword->text() != editKeystorePasswordConfirm->text()) {
        QMessageBox::warning(this, QString(), strPasswordMatch);
        editKeystorePasswordConfirm->setFocus();
        return false;
    }
    if (editAlias && editAlias->text().isEmpty()) {
        QMessageBox::warning(this, QString(), strFieldRequired);
        editAlias->setFocus();
        return false;
    }
    if (editKeyPassword && editKeyPassword->text().length() < 6) {
        QMessageBox::warning(this, QString(), strPasswordMinimum);
        editKeyPassword->setFocus();
        return false;
    }
    if (editKeyPassword && editKeyPassword->text() != editKeyPasswordConfirm->text()) {
        QMessageBox::warning(this, QString(), strPasswordMatch);
        editKeyPasswordConfirm->setFocus();
        return false;
    }
    if (editName && editName->text().isEmpty()) {
        QMessageBox::warning(this, QString(), strFieldRequired);
        editName->setFocus();
        return false;
    }
    return true;
}

QFormLayout *KeyCreator::createKeyLayout()
{
    QFormLayout *layout = new QFormLayout;
    layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

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

    layout->addRow(editAlias->placeholderText(), editAlias);
    layout->addRow(tr("Password"), layoutPassword);
    layout->addRow(tr("Validity (Years)"), editYears);
    layout->addRow(editName->placeholderText(), editName);
    layout->addRow(editUnit->placeholderText(), editUnit);
    layout->addRow(editOrganization->placeholderText(), editOrganization);
    layout->addRow(editCity->placeholderText(), editCity);
    layout->addRow(editState->placeholderText(), editState);
    layout->addRow(editCountry->placeholderText(), editCountry);

    return layout;
}
