#ifndef APKTOOL_H
#define APKTOOL_H

#include "tools/jar.h"

class Apktool : public Jar
{
public:
    explicit Apktool(const QString &jar, QObject *parent = nullptr) : Jar(jar, parent) {}

    void build(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources);
    void decode(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources);
    QString version() const;
    void reset() const;

private:
    void run(const QString &action, const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources);
};

#endif // APKTOOL_H
