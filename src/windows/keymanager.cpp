#include "windows/keymanager.h"
#include "windows/keystorecreator.h"
#include "windows/keyselector.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/password.h"
#include "base/settings.h"
#include "tools/keytool.h"
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

// KeyManager

KeyManager::KeyManager(QWidget *parent) : QDialog(parent)
{
    resize(Utils::scale(500, 0));
    //: This string refers to multiple keys (as in "Manager of keys").
    setWindowTitle(tr("Key Manager"));
    setWindowIcon(QIcon::fromTheme("apk-sign"));
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

    QHBoxLayout *layoutKeystore = new QHBoxLayout;
    layoutKeystore->addWidget(editKeystore);
    layoutKeystore->addWidget(btnCreateKeystore);

    QHBoxLayout *layoutKeyAlias = new QHBoxLayout;
    layoutKeyAlias->addWidget(editKeyAlias);
    layoutKeyAlias->addWidget(btnSelectKey);
    layoutKeyAlias->addWidget(btnCreateKey);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QFormLayout *customKeystoreLayout = new QFormLayout(groupKeystore);
    customKeystoreLayout->addRow(tr("Keystore path:"), layoutKeystore);
    customKeystoreLayout->addRow(tr("Keystore password:"), editKeystorePassword);
    customKeystoreLayout->addRow(tr("Key alias:"), layoutKeyAlias);
    customKeystoreLayout->addRow(tr("Key password:"), editKeyPassword);
    customKeystoreLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(groupKeystore);
    layout->addWidget(buttons);

    load();

    connect(btnCreateKeystore, &QToolButton::clicked, this, &KeyManager::createKeystore);
    connect(btnCreateKey, &QToolButton::clicked, this, &KeyManager::createKey);
    connect(btnSelectKey, &QToolButton::clicked, this, [this]() {
        const QString keystore = editKeystore->getCurrentPath();
        const QString password = editKeystorePassword->text();
        const QString defaultAlias = editKeyAlias->text();
        KeySelector keySelector(keystore, password, defaultAlias, this);
        const QString alias = keySelector.select();
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
    KeystoreCreator dialog(this);
    connect(&dialog, &KeystoreCreator::createdKeystore, this, [this](const QString &keystore) {
        if (!keystore.isEmpty()) {
            editKeystore->setCurrentPath(keystore);
        }
    });
    connect(&dialog, &KeystoreCreator::createdKey, this, [this](const QString &alias) {
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

    KeyCreator dialog(keystore, password, this);
    connect(&dialog, &KeyCreator::createdKey, this, [this](const QString &alias) {
        if (!alias.isEmpty()) {
            editKeyAlias->setText(alias);
        }
    });
    dialog.exec();
}
