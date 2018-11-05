#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"
#include <QFileDialog>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QPlainTextEdit>

namespace
{
    QString makePath(const QString &path)
    {
        return (!path.isEmpty() ? path : app->settings->getLastDirectory());
    }

    QString makeFilter(const QString &path, const FileFormatList &formats)
    {
        return (formats.isEmpty() ? FileFormatList(FileFormat::fromFilename(path)) : formats).getFilterString();
    }
}

QString Dialogs::getOpenFilename(QWidget *parent, const QString &defaultPath, const FileFormatList &formats)
{
    const QString path = makePath(defaultPath);
    const QString filter = makeFilter(defaultPath, formats);
    return QFileDialog::getOpenFileName(parent, QString(), path, filter);
}

QString Dialogs::getSaveFilename(QWidget *parent, const QString &defaultPath, const FileFormatList &formats)
{
    const QString path = makePath(defaultPath);
    const QString filter = makeFilter(defaultPath, formats);
    return QFileDialog::getSaveFileName(parent, QString(), path, filter);
}

QStringList Dialogs::getOpenFilenames(QWidget *parent, const QString &defaultPath, const FileFormatList &formats)
{
    const QString path = makePath(defaultPath);
    const QString filter = makeFilter(defaultPath, formats);
    return QFileDialog::getOpenFileNames(parent, QString(), path, filter);
}

QString Dialogs::getOpenImageFilename(QWidget *parent, const QString &defaultPath)
{
    return Dialogs::getOpenFilename(parent, defaultPath, FileFormatList::forReadableImages());
}

QString Dialogs::getSaveImageFilename(QWidget *parent, const QString &defaultPath)
{
    return Dialogs::getSaveFilename(parent, defaultPath, FileFormatList::forWritableImages());
}

QStringList Dialogs::getOpenImageFilenames(QWidget *parent, const QString &defaultPath)
{
    return Dialogs::getOpenFilenames(parent, defaultPath, FileFormatList::forReadableImages());
}

QStringList Dialogs::getOpenApkFilenames(QWidget *parent, const QString &defaultPath)
{
    return Dialogs::getOpenFilenames(parent, defaultPath, FileFormatList::forApk());
}

QString Dialogs::getSaveApkFilename(const Project *project, QWidget *parent, const QString &defaultPath)
{
    const QString directory = makePath(defaultPath);
    const QString filename = QFileInfo(project->getOriginalPath()).fileName();
    const QString path = QString("%1/%2").arg(directory, filename);
    return Dialogs::getSaveFilename(parent, path, FileFormatList::forApk());
}

QString Dialogs::getOpenKeystoreFilename(QWidget *parent, const QString &defaultPath)
{
    return Dialogs::getOpenFilename(parent, defaultPath, FileFormatList::forKeystore());
}

QString Dialogs::getSaveKeystoreFilename(QWidget *parent, const QString &defaultPath)
{
    return Dialogs::getSaveFilename(parent, defaultPath, FileFormatList::forKeystore());
}

bool Dialogs::openApk(QWidget *parent, const QString &defaultPath)
{
    const QStringList paths = getOpenApkFilenames(parent, defaultPath);
    if (paths.isEmpty()) {
        return false;
    }
    for (const QString &path : paths) {
        app->openApk(path);
    }
    return true;
}

QString Dialogs::getOpenDirectory(QWidget *parent, const QString &defaultPath)
{
    const QString path = makePath(defaultPath);
    return QFileDialog::getExistingDirectory(parent, QString(), path);
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

bool Dialogs::copyFile(const QString &src, QWidget *parent)
{
    const bool isReadableImage = Utils::isImageReadable(src);
    const QString dst = isReadableImage ? Dialogs::getSaveImageFilename(parent, src) : Dialogs::getSaveFilename(parent, src);
    if (dst.isEmpty()) {
        return false;
    }
    const bool isWritableImage = Utils::isImageWritable(dst);
    const bool isImage = (isReadableImage && isWritableImage);
    const bool success = isImage ? Utils::copyImage(src, dst) : Utils::copyFile(src, dst);
    if (!success) {
        QMessageBox::warning(parent, QString(), app->translate("Dialogs", "Could not save the file."));
        return false;
    }
    return true;
}

bool Dialogs::replaceFile(const QString &what, QWidget *parent)
{
    const bool isWritableImage = Utils::isImageWritable(what);
    const QString with = isWritableImage ? Dialogs::getOpenImageFilename(parent, what) : Dialogs::getOpenFilename(parent, what);
    if (with.isEmpty() || !QFile::exists(with)) {
        return false;
    }
    const bool isReadableImage = Utils::isImageReadable(with);
    const bool isImage = (isReadableImage && isWritableImage);
    const bool success = isImage ? Utils::copyImage(with, what) : Utils::copyFile(with, what);
    if (!success) {
        QMessageBox::warning(parent, QString(), app->translate("Dialogs", "Could not replace the file."));
        return false;
    }
    return true;
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
    dialog.resize(app->scale(600, 360));

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
