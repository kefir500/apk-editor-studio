#include "windows/keyselector.h"
#include "windows/dialogs.h"
#include "tools/keytool.h"
#include <QBoxLayout>
#include <QDialogButtonBox>

KeySelector::KeySelector(const QString &keystore, const QString &password,
                         const QString &defaultAlias, QWidget *parent)
    : QDialog(parent)
    , keystore(keystore)
    , password(password)
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

    refresh(defaultAlias);
}

QString KeySelector::select()
{
    return exec() == QDialog::Accepted ? getCurrentValue() : QString();
}

QString KeySelector::getCurrentValue() const
{
    return combo->currentText();
}

void KeySelector::refresh(const QString &defaultAlias)
{
    loading->show();
    auto keytool = new Keytool::Aliases(keystore, password, this);
    connect(keytool, &Keytool::Aliases::success, [=](const QStringList &aliases) {
        combo->clear();
        combo->addItems(aliases);
        combo->setCurrentText(defaultAlias);
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
