#include "windows/signatureviewer.h"
#include "widgets/loadingwidget.h"
#include "widgets/readonlycheckbox.h"
#include "tools/apksigner.h"
#include "base/utils.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTabWidget>

SignatureViewer::SignatureViewer(const QString &apkPath, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Signatures");
    setWindowIcon(QIcon::fromTheme("view-certificate"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(Utils::scale(640, 300));

    auto layout = new QVBoxLayout(this);

    //: Read more: https://source.android.com/security/apksigning#v1
    auto v1SchemeValue = new ReadOnlyCheckBox(tr("JAR signing"), this);
    layout->addWidget(v1SchemeValue);

    //: Read more: https://source.android.com/security/apksigning/v2
    auto v2SchemeValue = new ReadOnlyCheckBox(tr("APK Signature Scheme v2"), this);
    layout->addWidget(v2SchemeValue);

    //: Read more: https://source.android.com/security/apksigning/v3
    auto v3SchemeValue = new ReadOnlyCheckBox(tr("APK Signature Scheme v3"), this);
    layout->addWidget(v3SchemeValue);

    auto signerTabs = new QTabWidget(this);
    layout->addWidget(signerTabs);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    auto apksigner = new Apksigner::Verify(apkPath, this);
    apksigner->run();
    connect(apksigner, &Command::finished, [=](bool success) {
        if (success) {
            v1SchemeValue->setChecked(apksigner->hasV1Scheme());
            v2SchemeValue->setChecked(apksigner->hasV2Scheme());
            v3SchemeValue->setChecked(apksigner->hasV3Scheme());
            const auto signers = apksigner->signersInfo();
            for (int i = 0; i < signers.count(); ++i) {
                auto signerTab = new QPlainTextEdit(signers.at(i), this);
                signerTab->setReadOnly(true);
                signerTabs->addTab(signerTab, tr("Signer #%1").arg(i + 1));
            }
        } else {
            QMessageBox::warning(this, {}, tr("Could not retrieve the list of certificates."));
        }
        apksigner->deleteLater();
    });

    auto loading = new LoadingWidget(this);
    loading->show();
    connect(apksigner, &Command::finished, loading, &LoadingWidget::hide);
}
