#ifndef UTILS_H
#define UTILS_H

#include <QList>
#include <QPixmap>

namespace Utils
{
    // String utils:

    QString capitalize(QString string);

    // Math utils:

    int roundToNearest(int number, QList<int> numbers);

    // File / Directory utils:

    bool explore(const QString &path);
    void rmdir(const QString &path, bool recursive = false);
    bool copyFile(const QString &src, const QString &dst);
    bool copyImage(const QString &src, const QString &dst);

    // Image utils:

    bool isImageReadable(const QString &path);
    bool isImageWritable(const QString &path);
    QPixmap iconToPixmap(const QIcon &icon);

    // Android utils:

    QString getAndroidCodename(int api);
}

#endif // UTILS_H
