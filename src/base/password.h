#ifndef PASSWORD_H
#define PASSWORD_H

#include <QObject>

class Password
{
public:
    Password(const QString &key) : key(key) {}

    QString get();
    void set(const QString &value);
    void remove();

private:
    QString key;
};

#endif // PASSWORD_H
