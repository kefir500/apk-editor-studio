#include "editors/fileeditor.h"
#include "windows/dialogs.h"
#include "base/application.h"

FileEditor::FileEditor(const ResourceModelIndex &index, QWidget *parent) : Editor(parent), index(index)
{
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

    auto separator = [this]() -> QAction * {
        QAction *separator = new QAction(this);
        separator->setSeparator(true);
        return separator;
    };

    actionReplace = new QAction(app->icons.get("replace.png"), QString(), this);
    actionSaveAs = new QAction(app->icons.get("save-as.png"), QString(), this);
    actionRemove = new QAction(app->icons.get("remove.png"), QString(), this);
    actionExplore = new QAction(app->icons.get("explore.png"), QString(), this);

    actionReplace->setShortcut(QKeySequence("Ctrl+R"));
    actionSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));

    connect(actionReplace, &QAction::triggered, this, &FileEditor::replace);
    connect(actionSaveAs, &QAction::triggered, this, &FileEditor::saveAs);
    connect(actionRemove, &QAction::triggered, this, &FileEditor::remove);
    connect(actionExplore, &QAction::triggered, this, &FileEditor::explore);

    addAction(actionReplace);
    addAction(separator());
    addAction(actionSave);
    addAction(actionSaveAs);
    addAction(actionRemove);
    addAction(separator());
    addAction(actionExplore);

    retranslate();
}

bool FileEditor::saveAs()
{
    const QString filename = index.path();
    const QString destination = Dialogs::getSaveFilename(filename, this);
    if (destination.isEmpty()) {
        return false;
    }
    return save(destination);
}

bool FileEditor::replace()
{
    return index.replace();
}

bool FileEditor::remove()
{
    return index.remove();
}

bool FileEditor::explore() const
{
    return index.explore();
}

QStringList FileEditor::supportedFormats()
{
    return QStringList();
}

void FileEditor::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        Editor::changeEvent(event);
    }
}

void FileEditor::retranslate()
{
    actionReplace->setText(tr("&Replace Resource..."));
    actionSave->setText(tr("&Save Resource"));
    actionSaveAs->setText(tr("Save Resource &As..."));
    actionRemove->setText(tr("&Delete Resource"));
    //: This string refers to a single resource.
    actionExplore->setText(tr("&Open Resource Directory"));
}
