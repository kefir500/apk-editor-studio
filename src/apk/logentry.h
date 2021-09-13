#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <QColor>

class LogEntry
{
public:
    enum Type {
        Information,
        Success,
        Error
    };

    LogEntry(const QString &brief, Type type = Information);
    LogEntry(const QString &brief, const QString &descriptive, Type type = Information);

    QString getBrief() const;
    QString getDescriptive() const;
    Type getType() const;
    bool getLoading() const;
    QColor getColor() const;

    void setBrief(const QString &brief);
    void setDescriptive(const QString &descriptive);
    void setType(Type type);
    void setLoading(bool loading);

private:
    QString brief;
    QString descriptive;
    Type type;
    bool loading = false;
};

#endif // LOGENTRY_H
