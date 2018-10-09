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

    LogEntry(const QString &brief, const QString &descriptive, Type type);
    LogEntry(const QString &brief, const QString &descriptive, const QColor &color)
        : brief(brief), descriptive(descriptive), color(color) {}

    QString getBrief() const;
    QString getDescriptive() const;
    QColor getColor() const;

private:
    QString brief;
    QString descriptive;
    QColor color;
};

#endif // LOGENTRY_H
