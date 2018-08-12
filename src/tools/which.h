#ifndef WHICH_H
#define WHICH_H

#include "tools/executable.h"

class Which : public Executable {
public:
    Which(QObject *parent)
#ifndef Q_OS_WIN
        : Executable("which", parent = nullptr) {}
#else
        : Executable("where", parent = nullptr) {}
#endif
    Result<QString> find(const QString &application) const;
};

#endif // WHICH_H
