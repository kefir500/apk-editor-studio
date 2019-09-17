#ifndef ZIPALIGN_H
#define ZIPALIGN_H

#include "tools/executable.h"

class Zipalign : public Executable
{
public:
    explicit Zipalign(QObject *parent = nullptr) : Zipalign(getPath(), parent) {}
    explicit Zipalign(const QString &executable, QObject *parent = nullptr) : Executable(executable, parent) {}

    void align(const QString &apk);

    static QString getPath();
    static QString getDefaultPath();
};

#endif // ZIPALIGN_H
