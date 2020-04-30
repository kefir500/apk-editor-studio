#ifndef KEYSTORECREATOR_H
#define KEYSTORECREATOR_H

#include "windows/keycreator.h"

class KeystoreCreator : public KeyCreator
{
    Q_OBJECT

public:
    explicit KeystoreCreator(QWidget *parent = nullptr);

signals:
    void createdKeystore(const QString &path);

protected:
    void create() override;
    bool validate() override;

private:
    QLineEdit *editKeystorePassword;
    QLineEdit *editKeystorePasswordConfirm;
};

#endif // KEYSTORECREATOR_H
