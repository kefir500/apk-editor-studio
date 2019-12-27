#ifndef JAVAC_H
#define JAVAC_H

#include "tools/command.h"

namespace Javac
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

#endif // JAVAC_H
