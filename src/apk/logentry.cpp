#include "apk/logentry.h"
#include "base/utils.h"
#include <QPalette>

QString LogEntry::getBrief() const
{
    return brief;
}

QString LogEntry::getDescriptive() const
{
    return descriptive;
}

LogEntry::Type LogEntry::getType() const
{
    return type;
}

QColor LogEntry::getColor() const
{
    switch (type) {
    case Information:
        return QPalette().color(QPalette::Base);
    case Success:
        return Utils::isLightTheme() ? QColor(235, 250, 200) : QColor(75, 150, 90);
    case Error:
        return Utils::isLightTheme() ? QColor(255, 200, 200) : QColor(190, 95, 95);
    }
    return QPalette().color(QPalette::Base);
}
