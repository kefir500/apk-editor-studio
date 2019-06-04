#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <QColor>

class LogEntry
{
public:
    enum Type {
        Information,
        Success,
        Warning,
        Error
    };

    LogEntry(const QString &brief, const QString &descriptive, Type type)
        : brief(brief), descriptive(descriptive), type(type) {}

    QString getBrief() const;
    QString getDescriptive() const;
    Type getType() const;
    QColor getColor() const;

private:
    QString brief;
    QString descriptive;
    Type type;
};

#endif // LOGENTRY_H
