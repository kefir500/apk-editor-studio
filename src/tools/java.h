#ifndef JAVA_H
#define JAVA_H

#include "tools/command.h"

namespace Java
{
    class Version : public Command
    {
        Q_OBJECT

    public:
        Version(QObject *parent = nullptr) : Command(parent) {}
        void run() override;

    signals:
        void finished(const QString &version) const;
    };
}

#endif // JAVA_H
