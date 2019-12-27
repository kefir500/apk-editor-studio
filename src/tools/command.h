#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>

class Command : public QObject
{
public:
    Command(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~Command() = default;
    virtual void run() = 0;
};

#endif // COMMAND_H
