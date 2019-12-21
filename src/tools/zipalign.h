#ifndef ZIPALIGN_H
#define ZIPALIGN_H

#include <QObject>

class Zipalign : public QObject
{
    Q_OBJECT

public:
    explicit Zipalign(QObject *parent = nullptr) : QObject(parent) {}

    void align(const QString &apk);

    static QString getPath();
    static QString getDefaultPath();

signals:
    void alignFinished(bool success, const QString &message) const;
};

#endif // ZIPALIGN_H
