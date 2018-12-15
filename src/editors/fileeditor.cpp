#include "editors/fileeditor.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"

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

    actionReplace = new QAction(app->loadIcon("replace.png"), QString(), this);
    actionSaveAs = new QAction(app->loadIcon("save-as.png"), QString(), this);
    actionExplore = new QAction(app->loadIcon("explore.png"), QString(), this);

    actionReplace->setShortcut(QKeySequence("Ctrl+R"));
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

    retranslate();
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
    Utils::explore(index.path());
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
        QWidget::changeEvent(event);
    }
}

void FileEditor::retranslate()
{
    actionReplace->setText(tr("&Replace Resource..."));
    actionSave->setText(tr("&Save Resource"));
    actionSaveAs->setText(tr("Save Resource &As..."));
    actionExplore->setText(tr("&Open Resource Directory"));
}
