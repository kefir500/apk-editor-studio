#ifndef JAVAC_H
#define JAVAC_H

#include "base/command.h"

namespace Javac
{
    class Version : public Command
    {
    public:
        Version(QObject *parent = nullptr) : Command(parent) {}
        void run() override;
        const QString &version() const;

    private:
        QString resultVersion;
    };
}

#endif // JAVAC_H
