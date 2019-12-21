#include "windows/keymanager.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/password.h"
#include "tools/keytool.h"
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>

// KeyManager

KeyManager::KeyManager(QWidget *parent) : QDialog(parent)
{
    resize(app->scale(500, 0));
    //: This string refers to multiple keys (as in "Manager of keys").
    setWindowTitle(tr("Key Manager"));
    setWindowIcon(app->icons.get("key.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    groupKeystore = new QGroupBox(tr("Custom Keystore"), this);
    editKeystore = new FileBox(false, this);
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
    customKeystoreLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(groupKeystore);
    layout->addWidget(buttons);

    load();

    connect(btnCreateKeystore, &QToolButton::clicked, this, &KeyManager::createKeystore);
    connect(btnCreateKey, &QToolButton::clicked, this, &KeyManager::createKey);
    connect(btnSelectKey, &QToolButton::clicked, [this]() {
        const QString keystore = editKeystore->getCurrentPath();
        const QString password = editKeystorePassword->text();
        const QString alias = KeySelector::select(keystore, password, editKeyAlias->text(), this);
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

void KeyManager::createKeystore()
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
    dialog.exec();
}

void KeyManager::createKey()
{
    const QString keystore = editKeystore->getCurrentPath();
    const QString password = editKeystorePassword->text();

    auto keytool = new Keytool(this);
    connect(keytool, &Keytool::aliasesFetched, [=]() {
        KeyCreator dialog(keystore, password, this);
        connect(&dialog, &KeyCreator::createdKey, [&](const QString &alias) {
            if (!alias.isEmpty()) {
                editKeyAlias->setText(alias);
            }
        });
        dialog.exec();
        keytool->deleteLater();
    });
    keytool->fetchAliases(keystore, password);
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
    loading = new LoadingWidget(this);
    loading->hide();

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

void KeyCreator::create()
{
    if (!validateFields()) {
        return;
    }

    QString path;
    if (type == TypeKeystore) {
        path = Dialogs::getSaveKeystoreFilename();
        if (path.isEmpty()) {
            return;
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
    keystore.validity = editYears->text().toInt() * 365;
    keystore.dname.name = editName->text();
    keystore.dname.org = editOrganization->text();
    keystore.dname.orgUnit = editUnit->text();
    keystore.dname.city = editCity->text();
    keystore.dname.state = editState->text();
    keystore.dname.countryCode = editCountry->text();

    loading->show();
    auto keytool = new Keytool(this);
    connect(keytool, &Keytool::keystoreCreated, [=]() {
        loading->hide();
        if (type == TypeKeystore) {
            QMessageBox::information(this, QString(), tr("Keystore has been successfully created!"));
            emit createdKeystore(keystore.keystorePath);
            emit createdKey(keystore.keyAlias);
        } else if (type == TypeKey) {
            QMessageBox::information(this, QString(), tr("Key has been successfully created!"));
            emit createdKey(keystore.keyAlias);
        }
        accept();
        keytool->deleteLater();
    });
    connect(keytool, &Keytool::keystoreCreateError, [=](const QString &brief, const QString &detailed) {
        loading->hide();
        editAlias->setFocus();
        editAlias->selectAll();
        Dialogs::detailed(brief, detailed, QMessageBox::Warning, this);
        keytool->deleteLater();
    });
    keytool->createKeystore(keystore);
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

// KeyCreator

KeySelector::KeySelector(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Select Key"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    combo = new QComboBox(this);
    loading = new LoadingWidget(this);
    loading->hide();

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(combo);
    layout->addWidget(buttons);
}

QString KeySelector::select(const QString &keystore, const QString &password, const QString &currentAlias, QWidget *parent)
{
    KeySelector dialog(parent);
    dialog.refresh(keystore, password, currentAlias);
    return dialog.exec() == QDialog::Accepted ? dialog.getCurrentValue() : QString();
}

void KeySelector::refresh(const QString &keystore, const QString &password, const QString &currentAlias)
{
    loading->show();
    auto keytool = new Keytool(this);
    connect(keytool, &Keytool::aliasesFetched, [=](const QStringList &aliases) {
        combo->clear();
        combo->addItems(aliases);
        combo->setCurrentText(currentAlias);
        loading->hide();
        keytool->deleteLater();
    });
    connect(keytool, &Keytool::aliasesFetchError, [=](const QString &brief, const QString &detailed) {
        Dialogs::detailed(brief, detailed, QMessageBox::Warning, this);
        keytool->deleteLater();
    });
    keytool->fetchAliases(keystore, password);
}

QString KeySelector::getCurrentValue() const
{
    return combo->currentText();
}
