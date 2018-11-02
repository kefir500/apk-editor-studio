#ifndef UTILS_H
#define UTILS_H

#include <QList>

namespace Utils
{
    // String utils:

    QString capitalize(QString string);

    // Math utils:

    int roundToNearest(int number, QList<int> numbers);

    // File utils:

    bool copyFile(const QString &src, const QString &dst);
    bool copyImage(const QString &src, const QString &dst);

    bool isImageReadable(const QString &path);
    bool isImageWritable(const QString &path);
}

#endif // UTILS_H
