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

void BaseSheet::setSheetTitle(const QString &title)
{
    this->title = title;
    emit titleChanged(title);
}

void BaseSheet::setSheetIcon(const QIcon &icon)
{
    this->icon = icon;
    emit iconChanged(icon);
}

void BaseSheet::addActionSeparator()
{
    auto separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
}
