#include "windows/keymanager.h"
#include "windows/waitdialog.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/password.h"
#include "tools/keytool.h"
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QDialogButtonBox>

// KeyManager

KeyManager::KeyManager(QWidget *parent) : QDialog(parent)
{
    resize(app->dpiAwareSize(500, 0));
    setWindowTitle(tr("Key Manager"));
    setWindowIcon(app->loadIcon("key.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    groupKeystore = new QGroupBox(tr("Custom Keystore"), this);
    editKeystore = new FileBox(QString(), QString(), false, this);
    editKeystorePassword = new QLineEdit(this);
    editKeyAlias = new QLineEdit(this);
    editKeyPassword = new QLineEdit(this);
    btnCreateKeystore = new QPushButton(tr("Create"), this);
    btnCreateKey = new QPushButton(tr("Create"), this);
    btnSelectKey = new QToolButton(this);

    groupKeystore->setCheckable(true);
    btnSelectKey->setText("...");
    editKeystorePassword->setEchoMode(QLineEdit::Password);
    editKeyPassword->setEchoMode(QLineEdit::Password);

    QHBoxLayout *layoutKeyAlias = new QHBoxLayout;
    layoutKeyAlias->addWidget(editKeyAlias);
    layoutKeyAlias->addWidget(btnSelectKey);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QFormLayout *customKeystoreLayout = new QFormLayout(groupKeystore);
    customKeystoreLayout->addRow(tr("Keystore path:"), editKeystore);
    customKeystoreLayout->addWidget(btnCreateKeystore);
    customKeystoreLayout->addRow(tr("Keystore password:"), editKeystorePassword);
    customKeystoreLayout->addRow(tr("Key alias:"), layoutKeyAlias);
    customKeystoreLayout->addWidget(btnCreateKey);
    customKeystoreLayout->addRow(tr("Key password:"), editKeyPassword);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(groupKeystore);
    layout->addWidget(buttons);

    load();

    connect(btnCreateKeystore, &QToolButton::clicked, this, &KeyManager::createKeystore);
    connect(btnCreateKey, &QToolButton::clicked, this, &KeyManager::createKey);
    connect(btnSelectKey, &QToolButton::clicked, [this]() {
        const QString keystore = editKeystore->getCurrentPath();
        const QString password = editKeystorePassword->text();
        const QString alias = selectKey(keystore, password, editKeyAlias->text(), this);
        if (!alias.isEmpty()) {
            editKeyAlias->setText(alias);
        }
    });
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(this, &KeyManager::accepted, this, &KeyManager::save);
}

void KeyManager::load()
{
    groupKeystore->setChecked(app->settings->getCustomKeystore());
    editKeystore->setCurrentPath(app->settings->getKeystorePath());
    editKeyAlias->setText(app->settings->getKeyAlias());
    editKeystorePassword->setText(app->settings->getKeystorePassword());
    editKeyPassword->setText(app->settings->getKeyPassword());
}

void KeyManager::save()
{
    app->settings->setCustomKeystore(groupKeystore->isChecked());
    app->settings->setKeystorePath(editKeystore->getCurrentPath());
    app->settings->setKeyAlias(editKeyAlias->text());
    app->settings->setKeystorePassword(editKeystorePassword->text());
    app->settings->setKeyPassword(editKeyPassword->text());
}

bool KeyManager::createKeystore()
{
    KeyCreator dialog(this);
    connect(&dialog, &KeyCreator::createdKeystore, [&](const QString &keystore) {
        if (!keystore.isEmpty()) {
            editKeystore->setCurrentPath(keystore);
        }
    });
    connect(&dialog, &KeyCreator::createdKey, [&](const QString &alias) {
        if (!alias.isEmpty()) {
            editKeyAlias->setText(alias);
        }
    });
    return dialog.exec();
}

bool KeyManager::createKey()
{
    const QString keystore = editKeystore->getCurrentPath();
    const QString password = editKeystorePassword->text();
    const bool isKeystoreReadable = getCertificates(keystore, password, this).success;
    if (!isKeystoreReadable) {
        return false;
    }
    KeyCreator dialog(keystore, password, this);
    connect(&dialog, &KeyCreator::createdKey, [&](const QString &alias) {
        if (!alias.isEmpty()) {
            editKeyAlias->setText(alias);
        }
    });
    return dialog.exec();
}

QString KeyManager::selectKey(const QString &keystore, const QString &password, const QString &currentAlias, QWidget *parent)
{
    const QStringList certificates = getCertificates(keystore, password, parent).value;
    if (!certificates.isEmpty()) {
        const QString alias = Dialogs::combo(certificates, currentAlias, tr("Select Key"), parent);
        if (!alias.isEmpty()) {
            return alias;
        }
    }
    return QString();
}

Result<QStringList> KeyManager::getCertificates(const QString &keystore, const QString &password, QWidget *parent)
{
    Keytool keytool;
    WaitDialog *wait = new WaitDialog(parent);
    Q_UNUSED(wait)
    Result<QString> result = keytool.aliases(keystore, password);
    delete wait;
    if (!result.success) {
        QString text;
        if (result.value.contains("Keystore was tampered with, or password was incorrect")) {
            text = tr("Could not read keystore: incorrect password.");
        } else {
            text = tr("Could not read keystore.\nSee details for more information.");
        }
        Dialogs::detailed(text, result.value, QMessageBox::Warning, parent);
        return Result<QStringList>(false, QStringList());
    }

    QStringList certificates;
    QRegularExpression regex("^(.+),.+,\\s*PrivateKeyEntry,\\s*$");
    regex.setPatternOptions(QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator it = regex.globalMatch(result.value);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        certificates << match.captured(1);
    }
    return Result<QStringList>(true, certificates);
}

// KeyCreator

KeyCreator::KeyCreator(QWidget *parent) : QDialog(parent)
{
    initialize(TypeKeystore);
}

KeyCreator::KeyCreator(const QString &keystorePath, const QString &keystorePassword, QWidget *parent) : QDialog(parent)
{
    this->keystorePath = keystorePath;
    this->keystorePassword = keystorePassword;
    initialize(TypeKey);
}

QFormLayout *KeyCreator::initialize(Type type)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    this->type = type;

    QFormLayout *layout = new QFormLayout(this);
    QFormLayout *keyLayout = createKeyLayout();

    if (type == TypeKeystore) {
        editKeystorePassword = new QLineEdit(this);
        editKeystorePasswordConfirm = new QLineEdit(this);
        editKeystorePassword->setPlaceholderText(tr("Keystore Password"));
        editKeystorePasswordConfirm->setPlaceholderText(tr("Confirm Password"));
        editKeystorePassword->setEchoMode(QLineEdit::Password);
        editKeystorePasswordConfirm->setEchoMode(QLineEdit::Password);

        QHBoxLayout *layoutPassword = new QHBoxLayout;
        layoutPassword->addWidget(editKeystorePassword);
        layoutPassword->addWidget(editKeystorePasswordConfirm);
        layout->addRow(tr("Password"), layoutPassword);

        QGroupBox *groupKey = new QGroupBox(tr("Key"), this);
        groupKey->setLayout(keyLayout);
        layout->addRow(groupKey);

    } else if (type == TypeKey) {
        editKeystorePassword = nullptr;
        editKeystorePasswordConfirm = nullptr;
        layout->addRow(keyLayout);
    }

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &KeyCreator::create);
    connect(buttons, &QDialogButtonBox::rejected, this, &KeyCreator::reject);
    layout->addRow(buttons);

    return layout;
}

bool KeyCreator::create()
{
    if (!validateFields()) {
        return false;
    }

    QString path;
    if (type == TypeKeystore) {
        path = Dialogs::getSaveKeystoreFilename();
        if (path.isEmpty()) {
            return false;
        }
        QFile::remove(path);
    } else if (type == TypeKey) {
        path = keystorePath;
    }

    Keystore keystore;
    keystore.keystorePath = path;
    keystore.keystorePassword = keystorePassword.isEmpty() ? editKeystorePassword->text() : keystorePassword;
    keystore.keyAlias = editAlias->text();
    keystore.keyPassword = editKeyPassword->text();
    keystore.validity = editYears->text();
    keystore.dname.name = editName->text();
    keystore.dname.org = editOrganization->text();
    keystore.dname.orgUnit = editUnit->text();
    keystore.dname.city = editCity->text();
    keystore.dname.state = editState->text();
    keystore.dname.countryCode = editCountry->text();

    Keytool keytool;
    Result<QString> result = keytool.create(keystore);
    if (!result.success) {
        QString text;
        if (result.value.contains(QRegularExpression("alias <.+> already exists"))) {
            editAlias->setFocus();
            editAlias->selectAll();
            text = tr("Could not write to keystore: alias already exists.");
        } else {
            text = tr("Could not write to keystore.\nSee details for more information.");
        }
        Dialogs::detailed(text, result.value, QMessageBox::Warning, this);
        return false;
    }
    if (type == TypeKeystore) {
        QMessageBox::information(this, QString(), tr("Keystore has been successfully created!"));
        emit createdKeystore(keystore.keystorePath);
        emit createdKey(keystore.keyAlias);
    } else if (type == TypeKey) {
        QMessageBox::information(this, QString(), tr("Key has been successfully created!"));
        emit createdKey(keystore.keyAlias);
    }
    accept();
    return true;
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
