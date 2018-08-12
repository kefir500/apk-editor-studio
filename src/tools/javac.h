#ifndef JAVAC_H
#define JAVAC_H

#include "tools/executable.h"

class Javac : public Executable {

public:
    explicit Javac(QObject *parent = nullptr) : Executable("javac", parent) {}
    explicit Javac(const QString &path, QObject *parent = nullptr) : Executable(path, parent) {}
    QString version() const;
};

#endif // JAVAC_H
