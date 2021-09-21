#include "tools/keystore.h"
#include "base/application.h"
#include "base/settings.h"
#include "base/utils.h"
#include "windows/dialogs.h"
#include "windows/keyselector.h"
#include "windows/rememberdialog.h"
#include <QInputDialog>

std::unique_ptr<const Keystore> Keystore::get(QWidget *parent)
{
    auto keystore = std::unique_ptr<Keystore>(new Keystore);
    if (app->settings->getCustomKeystore()) {
        keystore->keystorePath = Utils::toAbsolutePath(app->settings->getKeystorePath());
        keystore->keystorePassword = app->settings->getKeystorePassword();
        keystore->keyAlias = app->settings->getKeyAlias();
        keystore->keyPassword = app->settings->getKeyPassword();
        if (keystore->keystorePath.isEmpty()) {
            keystore->keystorePath = Dialogs::getOpenKeystoreFilename();
            if (keystore->keystorePath.isEmpty()) {
                return nullptr;
            }
        }
        const auto inputDialogFlags = QDialog().windowFlags() & ~Qt::WindowContextHelpButtonHint;
        if (keystore->keystorePassword.isEmpty()) {
            bool accepted;
            keystore->keystorePassword = QInputDialog::getText(parent, QString(),
                                                               tr("Enter the keystore password:"),
                                                               QLineEdit::Password, QString(),
                                                               &accepted, inputDialogFlags);
            if (!accepted) {
                return nullptr;
            }
        }
        if (keystore->keyAlias.isEmpty()) {
            KeySelector keySelector(keystore->keystorePath, keystore->keystorePassword);
            keystore->keyAlias = keySelector.select();
            if (keystore->keyAlias.isEmpty()) {
                return nullptr;
            }
        }
        if (keystore->keyPassword.isEmpty()) {
            bool accepted;
            keystore->keyPassword = QInputDialog::getText(parent, QString(),
                                                          tr("Enter the key password:"),
                                                          QLineEdit::Password, QString(),
                                                          &accepted, inputDialogFlags);
            if (!accepted) {
                return nullptr;
            }
        }
    } else {
        RememberDialog::say("demo-keystore", tr(
            "You are using the built-in keystore provided for demonstrational "
            "purposes. It can be practical for testing or personal usage. "
            "However, if you plan to distribute this APK, we recommend you to "
            "specify/create your own keystore via Key Manager."
        ), parent);
        keystore->keystorePath = Utils::getSharedPath("tools/demo.jks");
        keystore->keystorePassword = "123456";
        keystore->keyAlias = "demo";
        keystore->keyPassword = "123456";
    }
    return keystore;
}
