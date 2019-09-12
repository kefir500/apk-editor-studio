#ifndef JAVAC_H
#define JAVAC_H

#include "tools/executable.h"

class Javac : public Executable
{
public:
    explicit Javac(QObject *parent = nullptr);
    explicit Javac(const QString &path, QObject *parent = nullptr) : Executable(path, parent) {}

    QString version() const;

private:
    static QString findJdkBinPath();
};

#endif // JAVAC_H
