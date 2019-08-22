#ifndef WHICH_H
#define WHICH_H

#include "tools/executable.h"

class Which : public Executable
{
public:
    Which(QObject *parent)
#ifndef Q_OS_WIN
        : Executable("which", parent) {}
#else
        : Executable("where", parent) {}
#endif
    Result<QString> find(const QString &application) const;
};

#endif // WHICH_H
