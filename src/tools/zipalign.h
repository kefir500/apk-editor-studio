#ifndef ZIPALIGN_H
#define ZIPALIGN_H

#include "tools/executable.h"

class Zipalign : public Executable {

public:
    explicit Zipalign(QObject *parent = nullptr) : Executable("zipalign", parent) {}
    explicit Zipalign(const QString &executable, QObject *parent = nullptr) : Executable(executable, parent) {}

    void align(const QString &apk);
};

#endif // ZIPALIGN_H
