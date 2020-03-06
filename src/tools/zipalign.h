#ifndef ZIPALIGN_H
#define ZIPALIGN_H

#include "base/command.h"

namespace Zipalign
{
    class Align : public Command
    {
    public:
        Align(const QString &apk, QObject *parent = nullptr) : Command(parent), apk(apk) {}
        void run() override;
        const QString &output() const;

    private:
        const QString apk;
        QString resultOutput;
    };

    QString getPath();
    QString getDefaultPath();
}

#endif // ZIPALIGN_H
