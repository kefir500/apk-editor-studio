#include "windows/waitdialog.h"
#include "base/application.h"

WaitDialog::WaitDialog(QWidget *parent) : QMessageBox(parent)
{
    const QString message = tr("Please wait...");
    parent->setEnabled(false);
    setWindowTitle(message);
    setText(message);
    setIconPixmap(app->loadPixmap("wait.png"));
    setStandardButtons(QMessageBox::NoButton);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setEnabled(true);
    show();
    app->processEvents();
}

WaitDialog::~WaitDialog()
{
    static_cast<QWidget *>(parent())->setEnabled(true);
}
