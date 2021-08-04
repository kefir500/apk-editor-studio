#include "windows/keystorecreator.h"
#include "windows/dialogs.h"
#include "tools/keytool.h"
#include <QGroupBox>
#include <QMessageBox>
#include <QFile>

KeystoreCreator::KeystoreCreator(QWidget *parent) : KeyCreator(QString(), QString(), parent)
{
    setWindowTitle(tr("Create Keystore"));

    editKeystorePassword = new QLineEdit(this);
    editKeystorePasswordConfirm = new QLineEdit(this);
    editKeystorePassword->setPlaceholderText(tr("Keystore Password"));
    editKeystorePasswordConfirm->setPlaceholderText(KeyCreator::tr("Confirm Password"));
    editKeystorePassword->setEchoMode(QLineEdit::Password);
    editKeystorePasswordConfirm->setEchoMode(QLineEdit::Password);
    editKeystorePassword->setFocus();

    loading->hide();

    QHBoxLayout *layoutPassword = new QHBoxLayout;
    layoutPassword->addWidget(editKeystorePassword);
    layoutPassword->addWidget(editKeystorePasswordConfirm);
    mainLayout->insertRow(0, KeyCreator::tr("Password"), layoutPassword);
}

void KeystoreCreator::create()
{
    if (!validate()) {
        return;
    }

    QString keystorePath = Dialogs::getSaveKeystoreFilename();
    if (keystorePath.isEmpty()) {
        return;
    }
    QFile::remove(keystorePath);

    Keystore keystore;
    fillKeystore(keystore, keystorePath, editKeystorePassword->text());

    loading->show();
    auto keytool = new Keytool::Genkey(keystore, this);
    connect(keytool, &Keytool::Genkey::success, this, [=]() {
        loading->hide();
        QMessageBox::information(this, QString(), tr("Keystore has been successfully created!"));
        emit createdKeystore(keystore.keystorePath);
        emit createdKey(keystore.keyAlias);
        accept();
        keytool->deleteLater();
    });
    connect(keytool, &Keytool::Genkey::error, this,
            [=](Keytool::Genkey::ErrorType, const QString &brief, const QString &detailed) {
        loading->hide();
        Dialogs::detailed(brief, detailed, QMessageBox::Warning, this);
        keytool->deleteLater();
    });
    keytool->run();
}

bool KeystoreCreator::validate()
{
    if (editKeystorePassword->text().length() < 6) {
        QMessageBox::warning(this, QString(), strPasswordMinimum);
        editKeystorePassword->setFocus();
        return false;
    }
    if (editKeystorePassword->text() != editKeystorePasswordConfirm->text()) {
        QMessageBox::warning(this, QString(), strPasswordMatch);
        editKeystorePasswordConfirm->setFocus();
        return false;
    }
    return KeyCreator::validate();
}
