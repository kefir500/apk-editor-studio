#include "sheets/basesheet.h"
#include <QAction>

const QString &BaseSheet::getTitle() const
{
    return title;
}

const QIcon &BaseSheet::getIcon() const
{
    return icon;
}

bool BaseSheet::finalize()
{
    return true;
}

void BaseSheet::addSeparator()
{
    auto separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
}
