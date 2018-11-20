#include "editors/baseeditor.h"
#include <QAction>

const QString &BaseEditor::getTitle() const
{
    return title;
}

const QIcon &BaseEditor::getIcon() const
{
    return icon;
}
