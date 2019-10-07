#ifndef APKTOOL_H
#define APKTOOL_H

#include "tools/jar.h"

class Apktool : public Jar
{
public:
    explicit Apktool(QObject *parent = nullptr) : Apktool(getPath(), parent) {}
    explicit Apktool(const QString &jar, QObject *parent = nullptr) : Jar(jar, parent) {}

    void decode(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources);
    void build(const QString &source, const QString &destination, const QString &frameworks);
    QString version() const;
    void reset() const;

    static QString getPath();
    static QString getDefaultPath();
    static QString getOutputPath();
    static QString getDefaultOutputPath();
    static QString getFrameworksPath();
    static QString getDefaultFrameworksPath();
};

#endif // APKTOOL_H
