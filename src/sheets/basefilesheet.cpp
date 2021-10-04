#include "sheets/basefilesheet.h"
#include "windows/dialogs.h"
#include <QAction>
#include <QEvent>

BaseFileSheet::BaseFileSheet(const ResourceModelIndex &index, QWidget *parent) : BaseEditableSheet(parent), index(index)
{
    setSheetIcon(index.icon());

    // Initialize file watcher:

    watcher.addPath(index.path());
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, [this]() {
        if (this->index.isValid()) {
            this->index.update();
            if (!isModified()) {
                load();
            }
        }
    });

    // Initialize menu actions:

    actionReplace = new QAction(QIcon::fromTheme("document-swap"), QString(), this);
    actionSaveAs = new QAction(QIcon::fromTheme("document-save-as"), QString(), this);
    actionExplore = new QAction(QIcon::fromTheme("folder-open"), QString(), this);

    actionReplace->setShortcut(QKeySequence("Ctrl+R"));
    actionSaveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));

    connect(actionReplace, &QAction::triggered, this, &BaseFileSheet::replace);
    connect(actionSaveAs, &QAction::triggered, this, &BaseFileSheet::saveAs);
    connect(actionExplore, &QAction::triggered, this, &BaseFileSheet::explore);

    addAction(actionReplace);
    addSeparator();
    addAction(actionSave);
    addAction(actionSaveAs);
    addSeparator();
    addAction(actionExplore);

    retranslate();
}

bool BaseFileSheet::saveAs()
{
    const QString filename = index.path();
    const QString destination = Dialogs::getSaveFilename(filename, this);
    if (destination.isEmpty()) {
        return false;
    }
    return save(destination);
}

bool BaseFileSheet::replace()
{
    return index.replace(this);
}

bool BaseFileSheet::explore() const
{
    return index.explore();
}

void BaseFileSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseEditableSheet::changeEvent(event);
}

void BaseFileSheet::retranslate()
{
    actionReplace->setText(tr("&Replace Resource..."));
    actionSave->setText(tr("&Save Resource"));
    actionSaveAs->setText(tr("Save Resource &As..."));
    //: This string refers to a single resource.
    actionExplore->setText(tr("&Open Resource Directory"));
}
