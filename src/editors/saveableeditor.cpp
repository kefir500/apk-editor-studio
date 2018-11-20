#include "editors/saveableeditor.h"
#include "base/application.h"
#include <QMessageBox>

SaveableEditor::SaveableEditor(QWidget *parent) : BaseEditor(parent)
{
    setModified(false);

    qDeleteAll(actions());
    actionSave = new QAction(app->loadIcon("save.png"), QString(), this);
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, [=]() { save(); });
    addAction(actionSave);

    retranslate();
}

bool SaveableEditor::commit()
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

bool SaveableEditor::isModified() const
{
    return modified;
}

void SaveableEditor::setModified(bool value)
{
    modified = value;
    emit savedStateChanged(!value);
}

void SaveableEditor::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        QWidget::changeEvent(event);
    }
}

void SaveableEditor::retranslate()
{
    actionSave->setText(tr("&Save"));
}
