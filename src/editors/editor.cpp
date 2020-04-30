#include "editors/editor.h"
#include "base/application.h"
#include <QMessageBox>

Editor::Editor(QWidget *parent) : Viewer(parent)
{
    setModified(false);

    qDeleteAll(actions());
    actionSave = new QAction(app->icons.get("save.png"), QString(), this);
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, [this]() {
        save();
    });
    addAction(actionSave);

    retranslate();
}

bool Editor::finalize()
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

bool Editor::isModified() const
{
    return modified;
}

void Editor::setModified(bool value)
{
    modified = value;
    emit modifiedStateChanged(modified);
}

void Editor::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    Viewer::changeEvent(event);
}

void Editor::retranslate()
{
    actionSave->setText(tr("&Save"));
}
