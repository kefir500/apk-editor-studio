#ifndef JARPROCESS_H
#define JARPROCESS_H

#include "base/process.h"

class JarProcess : public Process
{
    Q_OBJECT

public:
    JarProcess(QObject *parent = nullptr) : Process(parent) {}
    void run(const QString &jar, const QStringList &arguments = {}) override;
};

#endif // JARPROCESS_H
