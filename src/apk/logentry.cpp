#include "apk/logentry.h"
#include "base/application.h"
#include <QPalette>

LogEntry::LogEntry(const QString &brief, const QString &descriptive, Type type)
{
    this->brief = brief;
    this->descriptive = descriptive;

    switch (type) {
    case Information:
        this->color = QPalette().color(QPalette::Base);
        break;
    case Success:
        this->color = app->getColor(Application::ColorSuccess);
        break;
    case Warning:
        this->color = app->getColor(Application::ColorWarning);
        break;
    case Error:
        this->color = app->getColor(Application::ColorError);
        break;
    }
}

QString LogEntry::getBrief() const
{
    return brief;
}

QString LogEntry::getDescriptive() const
{
    return descriptive;
}

QColor LogEntry::getColor() const
{
    return color;
}
