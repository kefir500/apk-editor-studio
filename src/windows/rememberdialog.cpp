#include "windows/rememberdialog.h"
#include "base/application.h"
#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>

void RememberDialog::say(const QString &identifier, const QString &message, QWidget *parent)
{
    if (app->settings->hasRememberState(identifier)) {
        return;
    }

    auto checkboxRemember = new QCheckBox(tr("Don't show again"), parent);

    QMessageBox dialog(parent);
    dialog.setText(message);
    dialog.setCheckBox(checkboxRemember);
    dialog.addButton(QMessageBox::Ok);
    dialog.setIcon(QMessageBox::Icon::Information);
    dialog.connect(&dialog, &QDialog::accepted, [&]() {
        if (checkboxRemember->isChecked()) {
            app->settings->setRememberState(identifier, true);
        }
    });

    dialog.exec();
}

bool RememberDialog::ask(const QString &identifier, const QString &question, QWidget *parent)
{
    if (app->settings->hasRememberState(identifier)) {
        return app->settings->getRememberState(identifier);
    }

    bool result;
    auto checkboxRemember = new QCheckBox(tr("Don't show again"), parent);

    QMessageBox dialog(parent);
    dialog.setText(question);
    dialog.setCheckBox(checkboxRemember);
    dialog.addButton(QMessageBox::Yes);
    dialog.addButton(QMessageBox::No);
    dialog.setIcon(QMessageBox::Icon::Question);
    dialog.connect(&dialog, &QMessageBox::buttonClicked, [&](QAbstractButton *button) {
        const auto clickedButton = dialog.standardButton(button);
        if (clickedButton == QMessageBox::Yes) {
            result = true;
        } else if (clickedButton == QMessageBox::No) {
            result = false;
        }
        if (checkboxRemember->isChecked()) {
            app->settings->setRememberState(identifier, result);
        }
    });

    dialog.exec();
    return result;
}
