#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QQueue>

class Command : public QObject
{
    Q_OBJECT

public:
    Command(QObject *parent = nullptr);
    virtual void run() = 0;

signals:
    void started() const;
    void finished(bool success = true) const;
};

class Commands : public Command
{
public:
    Commands(QObject *parent = nullptr) : Command(parent) {}
    ~Commands() override;
    void run() override;
    void add(Command *command, bool critical = false);

private:
    void dequeue();
    QQueue<Command *> commands;
};

#endif // COMMAND_H
