#include "editors/baseeditor.h"
#include <QMessageBox>
#include <QAction>

BaseEditor::BaseEditor(QWidget *parent) : QWidget(parent)
{
    setModified(false);

    QAction *noAction = new QAction(tr("No actions"), this);
    noAction->setEnabled(false);
    addAction(noAction);
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

bool BaseEditor::isModified() const
{
    return modified;
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
