#ifndef ZIPALIGN_H
#define ZIPALIGN_H

#include "tools/command.h"

namespace Zipalign
{
    class Align : public Command
    {
        Q_OBJECT

    public:
        Align(const QString &apk, QObject *parent = nullptr) : Command(parent), apk(apk) {}
        void run() override;

    signals:
        void finished(bool success, const QString &output) const;

    private:
        const QString apk;
    };

    QString getPath();
    QString getDefaultPath();
}

#endif // ZIPALIGN_H
