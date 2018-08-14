#include "windows/dialogs.h"
#include "base/application.h"
#include "base/debug.h"
#include <QFileDialog>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QPlainTextEdit>

bool Dialogs::openApk(QWidget *parent, const QString &defaultPath)
{
    const QStringList paths = getOpenApkFilenames(parent, defaultPath);
    if (paths.isEmpty()) {
        return false;
    }
    foreach (const QString &path, paths) {
        app->openApk(path);
    }
    return true;
}

QStringList Dialogs::getOpenApkFilenames(QWidget *parent, const QString &defaultPath)
{
    const QString caption = app->translate("Dialogs", "Open APK");
    const QString filter = app->formats.filterApk();
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getOpenFileNames(parent, caption, path, filter);
}

QString Dialogs::getSaveApkFilename(const Project *project, QWidget *parent, const QString &defaultPath)
{
    const QString caption = app->translate("Dialogs", "Save APK");
    const QString filter = app->formats.filterApk();
    const QString directory = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    const QString filename = QFileInfo(project->getOriginalPath()).fileName();
    const QString path = QString("%1/%2").arg(directory, filename);
    return QFileDialog::getSaveFileName(parent, caption, path, filter);
}

QString Dialogs::getOpenImageFilename(QWidget *parent, const QString &defaultPath)
{
    const QString caption = app->translate("Dialogs", "Open Image");
    const QString filter = app->formats.filterImages();
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getOpenFileName(parent, caption, path, filter);
}

QStringList Dialogs::getOpenImageFilenames(QWidget *parent, const QString &defaultPath)
{
    const QString caption = app->translate("Dialogs", "Open Images");
    const QString filter = app->formats.filterImages();
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getOpenFileNames(parent, caption, path, filter);
}

QString Dialogs::getOpenFilename(QWidget *parent, const QString &defaultPath, bool filterExtension)
{
    const QString caption = app->translate("Dialogs", "Open File");
    const QString filter = !filterExtension ? app->formats.filterAllFiles() : app->formats.filterExtension(QFileInfo(defaultPath).suffix());
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getOpenFileName(parent, caption, path, filter);
}

QStringList Dialogs::getOpenFilenames(QWidget *parent, const QString &defaultPath, bool filterExtension)
{
    const QString caption = app->translate("Dialogs", "Open Files");
    const QString filter = !filterExtension ? app->formats.filterAllFiles() : app->formats.filterExtension(QFileInfo(defaultPath).suffix());
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getOpenFileNames(parent, caption, path, filter);
}

QString Dialogs::getOpenDirectory(QWidget *parent, const QString &defaultPath)
{
    const QString caption = QString();
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getExistingDirectory(parent, caption, path);
}

QString Dialogs::getOpenKeystoreFilename(QWidget *parent, const QString &defaultPath)
{
    const QString caption = app->translate("Dialogs", "Open Keystore");
    const QString filter = app->formats.filterKeystore();
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getOpenFileName(parent, caption, path, filter);
}

QString Dialogs::getSaveKeystoreFilename(QWidget *parent, const QString &defaultPath)
{
    const QString caption = app->translate("Dialogs", "Save Keystore");
    const QString filter = app->formats.filterKeystore();
    const QString path = defaultPath.isEmpty() ? app->settings->getLastDirectory() : defaultPath;
    return QFileDialog::getSaveFileName(parent, caption, path, filter);
}

QString Dialogs::combo(const QStringList &options, QWidget *parent)
{
    return Dialogs::combo(options, QString(), QString(), parent);
}

QString Dialogs::combo(const QStringList &options, const QString &current, QWidget *parent)
{
    return Dialogs::combo(options, current, QString(), parent);
}

QString Dialogs::combo(const QStringList &options, const QString &current, const QString &title, QWidget *parent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QComboBox *combo = new QComboBox(&dialog);
    combo->addItems(options);
    combo->setCurrentText(current);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    app->connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    app->connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(combo);
    layout->addWidget(buttons);

    return dialog.exec() == QDialog::Accepted ? combo->currentText() : QString();
}

bool Dialogs::replaceFile(const QString &path, QWidget *parent)
{
    const QString newfile = Dialogs::getOpenFilename(parent, path, true);
    if (newfile.isEmpty() || !QFile::exists(newfile)) {
        return false;
    }
    QFile::remove(path);
    const bool success = QFile::copy(newfile, path);
    if (!success) {
        QMessageBox::warning(parent, QString(), app->translate("Dialogs", "Could not replace the file."));
    }
    return success;
}

int Dialogs::detailed(const QString &text, const QString &detailed, QMessageBox::Icon icon, QWidget *parent)
{
    QMessageBox dialog(parent);
    dialog.setText(text);
    dialog.setDetailedText(detailed);
    dialog.setIcon(icon);
    dialog.setStandardButtons(QMessageBox::Ok);
    dialog.setEscapeButton(QMessageBox::Ok);
    return dialog.exec();
}

int Dialogs::log(const QString &text, QWidget *parent)
{
    QDialog dialog(parent);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.resize(app->dpiAwareSize(600, 360));

    QPlainTextEdit *textBox = new QPlainTextEdit(text, &dialog);
    textBox->setReadOnly(true);
    textBox->setPlainText(text);
    textBox->setLineWrapMode(QPlainTextEdit::NoWrap);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    textBox->setFont(font);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    app->connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    app->connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(textBox);
    layout->addWidget(buttons);

    return dialog.exec();
}
