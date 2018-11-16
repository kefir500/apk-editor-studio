#include "editors/fileeditor.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"

FileEditor::FileEditor(const ResourceModelIndex &index, QWidget *parent) : BaseEditor(parent), index(index)
{
    setModified(false);
    icon = index.icon();
    watcher.addPath(index.path());
    connect(&watcher, &QFileSystemWatcher::fileChanged, [this]() {
        this->index.update();
        watcher.addPath(this->index.path());
        if (!isModified()) {
            load();
        }
    });
}

bool FileEditor::saveAs()
{
    const QString filename = index.path();
    const bool isImage = Utils::isImageReadable(filename);
    const QString destination = isImage ? Dialogs::getSaveImageFilename(this, filename) : Dialogs::getSaveFilename(this, filename); // TODO lol wtf why not encpsulate in ImageEditor
    if (destination.isEmpty()) {
        return false;
    }
    return save(destination);
}

bool FileEditor::replace()
{
    const QString path = index.path();
    return Dialogs::replaceFile(path, this);
}

void FileEditor::explore() const
{
    app->explore(index.path());
}

QStringList FileEditor::supportedFormats()
{
    return QStringList();
}
