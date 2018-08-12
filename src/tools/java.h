#ifndef JAVA_H
#define JAVA_H

#include "tools/executable.h"

class Java : public Executable {

public:
    explicit Java(QObject *parent = nullptr) : Executable("java", parent) {}
    explicit Java(const QString &path, QObject *parent) : Executable(path, parent = nullptr) {}

    QString version();
};

#endif // JAVA_H
