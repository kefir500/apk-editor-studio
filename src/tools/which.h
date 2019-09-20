#ifndef WHICH_H
#define WHICH_H

#include "tools/executable.h"

class Which : public Executable
{
public:
    static Result<QString> find(const QString &application);

private:
    Which(QObject *parent = nullptr)
#ifndef Q_OS_WIN
        : Executable("which", parent) {}
#else
        : Executable("where", parent) {}
#endif
};

#endif // WHICH_H
