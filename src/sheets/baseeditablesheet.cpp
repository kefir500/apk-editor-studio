#include "sheets/baseeditablesheet.h"
#include <QAction>
#include <QEvent>
#include <QMessageBox>

BaseEditableSheet::BaseEditableSheet(QWidget *parent) : BaseSheet(parent)
{
    setModified(false);

    actionSave = new QAction(QIcon::fromTheme("document-save"), QString(), this);
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, this, [this]() {
        save();
    });
    addAction(actionSave);

    retranslate();
}

bool BaseEditableSheet::finalize()
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

bool BaseEditableSheet::isModified() const
{
    return modified;
}

void BaseEditableSheet::setModified(bool value)
{
    modified = value;
    emit modifiedStateChanged(modified);
}

void BaseEditableSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseSheet::changeEvent(event);
}

void BaseEditableSheet::retranslate()
{
    actionSave->setText(tr("&Save"));
}
