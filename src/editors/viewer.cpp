#include "editors/viewer.h"

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
