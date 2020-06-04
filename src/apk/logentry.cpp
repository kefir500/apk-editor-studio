#include "apk/logentry.h"
#include "base/application.h"
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
        return app->theme()->color(Theme::Color::Success);
    case Error:
        return app->theme()->color(Theme::Color::Error);
    }
    return QPalette().color(QPalette::Base);
}
