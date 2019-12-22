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

QString Dialogs::getOpenFilename(const QString &defaultPath, QWidget *parent)
{
    return getOpenFilename(defaultPath, FileFormatList(), parent);
}

QString Dialogs::getOpenFilename(const QString &defaultPath, const FileFormatList &formats, QWidget *parent)
{
    const QString path = makePath(defaultPath);
    const QString filter = makeFilter(defaultPath, formats);
    return QFileDialog::getOpenFileName(parent, QString(), path, filter);
}

QString Dialogs::getSaveFilename(const QString &defaultPath, QWidget *parent)
{
    return getSaveFilename(defaultPath, FileFormatList(), parent);
}

QString Dialogs::getSaveFilename(const QString &defaultPath, const FileFormatList &formats, QWidget *parent)
{
    const QString path = makePath(defaultPath);
    const QString filter = makeFilter(defaultPath, formats);
    QString defaultFilter = FileFormat::fromFilename(path).getFilterString();
    return QFileDialog::getSaveFileName(parent, QString(), path, filter, &defaultFilter);
}

QStringList Dialogs::getOpenFilenames(const QString &defaultPath, const FileFormatList &formats, QWidget *parent)
{
    const QString path = makePath(defaultPath);
    const QString filter = makeFilter(defaultPath, formats);
    return QFileDialog::getOpenFileNames(parent, QString(), path, filter);
}

QString Dialogs::getOpenImageFilename(QWidget *parent)
{
    return Dialogs::getOpenImageFilename(QString(), parent);
}

QString Dialogs::getOpenImageFilename(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getOpenFilename(defaultPath, FileFormatList::forReadableImages(), parent);
}

QString Dialogs::getSaveImageFilename(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getSaveFilename(defaultPath, FileFormatList::forWritableImages(), parent);
}

QStringList Dialogs::getOpenImageFilenames(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getOpenFilenames(defaultPath, FileFormatList::forReadableImages(), parent);
}

QStringList Dialogs::getOpenApkFilenames(QWidget *parent)
{
    return Dialogs::getOpenApkFilenames(QString(), parent);
}

QStringList Dialogs::getOpenApkFilenames(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getOpenFilenames(defaultPath, FileFormatList::forApk(), parent);
}

QString Dialogs::getSaveApkFilename(const Project *project, QWidget *parent)
{
    return Dialogs::getSaveApkFilename(project, QString(), parent);
}

QString Dialogs::getSaveApkFilename(const Project *project, const QString &defaultPath, QWidget *parent)
{
    const QString directory = makePath(defaultPath);
    const QString filename = QFileInfo(project->getOriginalPath()).fileName();
    const QString path = QString("%1/%2").arg(directory, filename);
    return Dialogs::getSaveFilename(path, FileFormatList::forApk(), parent);
}

QString Dialogs::getOpenKeystoreFilename(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getOpenFilename(defaultPath, FileFormatList::forKeystore(), parent);
}

QString Dialogs::getSaveKeystoreFilename(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getSaveFilename(defaultPath, FileFormatList::forKeystore(), parent);
}

bool Dialogs::openApk(QWidget *parent)
{
    return Dialogs::openApk(QString(), parent);
}

bool Dialogs::openApk(const QString &defaultPath, QWidget *parent)
{
    const QStringList paths = getOpenApkFilenames(defaultPath, parent);
    if (paths.isEmpty()) {
        return false;
    }
    for (const QString &path : paths) {
        app->openApk(path);
    }
    return true;
}

QString Dialogs::getOpenDirectory(const QString &defaultPath, QWidget *parent)
{
    const QString path = makePath(defaultPath);
    return QFileDialog::getExistingDirectory(parent, QString(), path);
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

int Dialogs::log(const QString &title, const QString &text, QWidget *parent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.resize(app->scale(600, 360));

    QPlainTextEdit *textBox = new QPlainTextEdit(text, &dialog);
    textBox->setReadOnly(true);
    textBox->setPlainText(text);
    textBox->setLineWrapMode(QPlainTextEdit::NoWrap);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    textBox->setFont(font);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    QPushButton *btnCopy = new QPushButton(app->translate("Dialogs", "Copy to Clipboard"), buttons);
    buttons->addButton(btnCopy, QDialogButtonBox::ActionRole);
    app->connect(btnCopy, &QPushButton::clicked, [=]() {
        textBox->selectAll();
        textBox->copy();
    });
    app->connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    app->connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(textBox);
    layout->addWidget(buttons);

    return dialog.exec();
}

int Dialogs::log(const QString &text, QWidget *parent)
{
    return log(QString(), text, parent);
}
