#include "editors/fileeditor.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"

FileEditor::FileEditor(const ResourceModelIndex &index, QWidget *parent) : BaseEditor(parent), index(index)
{
    setModified(false);
    icon = index.icon();

    // Initialize file watcher:

    watcher.addPath(index.path());
    connect(&watcher, &QFileSystemWatcher::fileChanged, [this]() {
        this->index.update();
        watcher.addPath(this->index.path());
        if (!isModified()) {
            load();
        }
    });

    // Initialize menu actions:

    actions().clear();

    auto separator = [this]() -> QAction * {
        QAction *separator = new QAction(this);
        separator->setSeparator(true);
        return separator;
    };

    QAction *actionReplace = new QAction(app->loadIcon("replace.png"), tr("&Replace Resource..."), this);
    QAction *actionSave = new QAction(app->loadIcon("save.png"), tr("&Save Resource"), this);
    QAction *actionSaveAs = new QAction(app->loadIcon("save-as.png"), tr("Save Resource &As..."), this);
    QAction *actionExplore = new QAction(app->loadIcon("explore.png"), tr("&Open Resource Directory"), this);

    actionReplace->setShortcut(QKeySequence("Ctrl+R"));
    actionSave->setShortcut(QKeySequence::Save);
    actionSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));

    connect(actionReplace, &QAction::triggered, this, &FileEditor::replace);
    connect(actionSaveAs, &QAction::triggered, this, &FileEditor::saveAs);
    connect(actionExplore, &QAction::triggered, this, &FileEditor::explore);

    addAction(actionReplace);
    addAction(separator());
    addAction(actionSave);
    addAction(actionSaveAs);
    addAction(separator());
    addAction(actionExplore);
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
