#include "editors/viewer.h"
#include <QAction>

const QString &Viewer::getTitle() const
{
    return title;
}

const QIcon &Viewer::getIcon() const
{
    return icon;
}

bool Viewer::finalize()
{
    return true;
}

void Viewer::addSeparator()
{
    auto separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
}
