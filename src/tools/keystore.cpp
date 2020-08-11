#include "tools/keystore.h"
#include "base/application.h"
#include "windows/dialogs.h"
#include "windows/keyselector.h"
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
        if (keystore->keystorePassword.isEmpty()) {
            bool accepted;
            keystore->keystorePassword = QInputDialog::getText(parent, QString(), tr("Enter the keystore password:"), QLineEdit::Password, QString(), &accepted);
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
            keystore->keyPassword = QInputDialog::getText(parent, QString(), tr("Enter the key password:"), QLineEdit::Password, QString(), &accepted);
            if (!accepted) {
                return nullptr;
            }
        }
    } else {
        // This keystore is provided for demonstrational purposes.
        keystore->keystorePath = Utils::getSharedPath("tools/demo.jks");
        keystore->keystorePassword = "123456";
        keystore->keyAlias = "demo";
        keystore->keyPassword = "123456";
    }
    return keystore;
}
