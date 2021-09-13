#include "apk/logentry.h"
#include "base/utils.h"
#include <QPalette>

LogEntry::LogEntry(const QString &brief, Type type)
{
    setBrief(brief);
    setType(type);
}

LogEntry::LogEntry(const QString &brief, const QString &descriptive, Type type)
{
    setBrief(brief);
    setDescriptive(descriptive);
    setType(type);
}

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

bool LogEntry::getLoading() const
{
    return loading;
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

void LogEntry::setBrief(const QString &brief)
{
    this->brief = brief;
}

void LogEntry::setDescriptive(const QString &descriptive)
{
    this->descriptive = descriptive;
}

void LogEntry::setType(Type type)
{
    this->type = type;
    this->loading = type == Type::Information;
}

void LogEntry::setLoading(bool loading)
{
    this->loading = loading;
}
