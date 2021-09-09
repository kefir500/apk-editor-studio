#include "windows/dialogs.h"
#include "windows/devicemanager.h"
#include "base/application.h"
#include "base/settings.h"
#include "base/utils.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QPushButton>

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

QString Dialogs::getOpenFilename(QWidget *parent)
{
    return getOpenFilename({}, FileFormatList(), parent);
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
    auto paths = Dialogs::getOpenFilenames(QString(), FileFormatList::forApk(), parent);
    if (!paths.isEmpty()) {
        app->settings->setLastDirectory(QFileInfo(paths.last()).absolutePath());
    }
    return paths;
}

QString Dialogs::getSaveApkFilename(const Project *project, QWidget *parent)
{
    const QString defaultPath = project->getOriginalPath();
    return Dialogs::getSaveFilename(defaultPath, FileFormatList::forApk(), parent);
}

QString Dialogs::getOpenKeystoreFilename(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getOpenFilename(defaultPath, FileFormatList::forKeystore(), parent);
}

QString Dialogs::getSaveKeystoreFilename(const QString &defaultPath, QWidget *parent)
{
    return Dialogs::getSaveFilename(defaultPath, FileFormatList::forKeystore(), parent);
}

QString Dialogs::getOpenDirectory(const QString &defaultPath, QWidget *parent)
{
    const QString path = makePath(defaultPath);
    return QFileDialog::getExistingDirectory(parent, QString(), path);
}

Device Dialogs::getInstallDevice(QWidget *parent)
{
    const QString title(qApp->translate("Dialogs", "Install APK"));
    const QString action(qApp->translate("Dialogs", "Install"));
    const QIcon icon(QIcon::fromTheme("apk-install"));
    return DeviceManager::selectDevice(title, action, icon, parent);
}

Device Dialogs::getExplorerDevice(QWidget *parent)
{
    const QString action(qApp->translate("AndroidExplorer", "Android Explorer"));
    const QIcon icon(QIcon::fromTheme("tool-androidexplorer"));
    return DeviceManager::selectDevice({}, action, icon, parent);
}

Device Dialogs::getScreenshotDevice(QWidget *parent)
{
    const QString action(qApp->translate("Dialogs", "Screenshot"));
    const QIcon icon(QIcon::fromTheme("camera-photo"));
    return DeviceManager::selectDevice({}, action, icon, parent);
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
    dialog.resize(Utils::scale(600, 360));

    QPlainTextEdit *textBox = new QPlainTextEdit(text, &dialog);
    textBox->setReadOnly(true);
    textBox->setPlainText(text);
    textBox->setLineWrapMode(QPlainTextEdit::NoWrap);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    textBox->setFont(font);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    QPushButton *btnCopy = new QPushButton(qApp->translate("Dialogs", "Copy to Clipboard"), buttons);
    buttons->addButton(btnCopy, QDialogButtonBox::ActionRole);
    qApp->connect(btnCopy, &QPushButton::clicked, parent, [=]() {
        textBox->selectAll();
        textBox->copy();
    });
    qApp->connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    qApp->connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(textBox);
    layout->addWidget(buttons);

    return dialog.exec();
}

int Dialogs::log(const QString &text, QWidget *parent)
{
    return log(QString(), text, parent);
}
