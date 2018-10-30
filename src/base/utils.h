#ifndef UTILS_H
#define UTILS_H

#include <QList>

namespace Utils
{
    // String utils:

    QString capitalize(QString string);

    // File utils:

    bool copyFile(const QString &src, const QString &dst);
    bool copyImage(const QString &src, const QString &dst);
}

#endif // UTILS_H
