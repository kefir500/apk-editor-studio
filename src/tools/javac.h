#ifndef JAVAC_H
#define JAVAC_H

#include <QObject>

class Javac : public QObject
{
    Q_OBJECT

public:
    explicit Javac(QObject *parent = nullptr) : QObject(parent) {}
    void version();

signals:
    void versionFetched(const QString &version) const;
};

#endif // JAVAC_H
