#ifndef JAVA_H
#define JAVA_H

#include "tools/executable.h"

class Java : public Executable
{
public:
    explicit Java(QObject *parent = nullptr);
    explicit Java(const QString &path, QObject *parent = nullptr) : Executable(path, parent) {}

    QString version();
};

#endif // JAVA_H
