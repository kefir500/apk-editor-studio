#ifndef JAVA_H
#define JAVA_H

#include <QObject>

class Java : public QObject
{
    Q_OBJECT

public:
    explicit Java(QObject *parent = nullptr) : QObject(parent) {}
    void version();

signals:
    void versionFetched(const QString &version) const;
};

#endif // JAVA_H
