#include "base/command.h"

Command::Command(QObject *parent) : QObject(parent)
{
    QObject::connect(this, &Command::finished, this, &Command::deleteLater);
}

Commands::~Commands()
{
    for (auto command : qAsConst(commands)) {
        command->deleteLater();
    }
}

void Commands::run()
{
    emit started();
    dequeue();
}

void Commands::add(Command *command, bool critical)
{
    commands.enqueue(command);
    connect(command, &Command::finished, this, [=](bool success) {
        if (success || !critical) {
            dequeue();
        } else {
            emit finished(false);
        }
    });
}

void Commands::dequeue()
{
    if (!commands.isEmpty()) {
        auto command = commands.dequeue();
        command->run();
    } else {
        emit finished(true);
    }
}
