#include "windows/keyselector.h"
#include "windows/dialogs.h"
#include "tools/keytool.h"
#include <QBoxLayout>
#include <QDialogButtonBox>

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
    auto keytool = new Keytool::Aliases(keystore, password, this);
    connect(keytool, &Keytool::Aliases::success, [=](const QStringList &aliases) {
        combo->clear();
        combo->addItems(aliases);
        combo->setCurrentText(currentAlias);
        loading->hide();
        keytool->deleteLater();
    });
    connect(keytool, &Keytool::Aliases::error, [=](const QString &brief, const QString &detailed) {
        Dialogs::detailed(brief, detailed, QMessageBox::Warning, this);
        keytool->deleteLater();
    });
    keytool->run();
}

QString KeySelector::getCurrentValue() const
{
    return combo->currentText();
}
