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
        return app->getColor(Application::ColorSuccess);
    case Warning:
        return app->getColor(Application::ColorWarning);
    case Error:
        return app->getColor(Application::ColorError);
    }
}
