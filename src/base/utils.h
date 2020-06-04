#ifndef UTILS_H
#define UTILS_H

#include <QList>
#include <QPixmap>
#include <QFileInfo>

namespace Utils
{
    enum AndroidSDKLevel {
        ANDROID_3 = 3,
        ANDROID_4,
        ANDROID_5,
        ANDROID_6,
        ANDROID_7,
        ANDROID_8,
        ANDROID_9,
        ANDROID_10,
        ANDROID_11,
        ANDROID_12,
        ANDROID_13,
        ANDROID_14,
        ANDROID_15,
        ANDROID_16,
        ANDROID_17,
        ANDROID_18,
        ANDROID_19,
        ANDROID_20,
        ANDROID_21,
        ANDROID_22,
        ANDROID_23,
        ANDROID_24,
        ANDROID_25,
        ANDROID_26,
        ANDROID_27,
        ANDROID_28,
        ANDROID_29,
        ANDROID_SDK_COUNT
    };

    // String utils:

    QString capitalize(QString string);

    // Math utils:

    int roundToNearest(int number, QList<int> numbers);

    // Color utils:

    bool isDark(const QColor &color);
    bool isDarkTheme();

    // File / Directory utils:

    bool explore(const QString &path);
    void rmdir(const QString &path, bool recursive = false);
    bool copyFile(const QString &src);
    bool copyFile(const QString &src, QString dst);
    bool replaceFile(const QString &what);
    bool replaceFile(const QString &what, QString with);
    QString normalizePath(QString path);

    // Image utils:

    bool isImageReadable(const QString &path);
    bool isImageWritable(const QString &path);
    QPixmap iconToPixmap(const QIcon &icon);

    // Android utils:

    bool isDrawableResource(const QFileInfo &file);
    QString getAndroidCodename(int api);
}

#endif // UTILS_H
