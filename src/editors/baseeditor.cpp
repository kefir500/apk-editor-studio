#include "editors/baseeditor.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"
#include <QMessageBox>

BaseEditor::BaseEditor(const QString &filename, const QPixmap &icon, QWidget *parent) : QWidget(parent)
{
    this->filename = filename;
    this->icon = icon;

    setModified(false);

    if (!filename.isNull() && QFile::exists(filename)) {
        watcher.addPath(filename);
        connect(&watcher, &QFileSystemWatcher::fileChanged, [this]() {
            if (!isModified()) {
                load();
            }
        });
    }
}

bool BaseEditor::saveAs()
{
    if (filename.isEmpty()) {
        return false;
    }
    const bool isImage = Utils::isImageReadable(filename);
    const QString destination = isImage ? Dialogs::getSaveImageFilename(this, filename) : Dialogs::getSaveFilename(this, filename);
    if (destination.isEmpty()) {
        return false;
    }
    return save(destination);
}

bool BaseEditor::commit()
{
    if (isModified()) {
        const int answer = QMessageBox::question(this, QString(), tr("Save the changes?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch (answer) {
            case QMessageBox::Yes:
                save();
                return true;
            case QMessageBox::No:
                return true;
            default:
                return false;
        }
    }
    return true;
}

bool BaseEditor::replace()
{
    return Dialogs::replaceFile(filename, this);
}

void BaseEditor::explore()
{
    app->explore(filename);
}

bool BaseEditor::isModified() const
{
    return modified;
}

bool BaseEditor::isEditable() const
{
    return !filename.isEmpty();
}

const QString &BaseEditor::getTitle() const
{
    return title;
}

const QIcon &BaseEditor::getIcon() const
{
    return icon;
}

void BaseEditor::setModified(bool value)
{
    modified = value;
    emit savedStateChanged(!value);
}

QString BaseEditor::filter()
{
    const QString suffix = QFileInfo(filename).suffix();
    const QString filters = app->formats.filterExtension(suffix);
    return filters;
}
