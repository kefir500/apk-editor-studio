#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>

class Process : public QObject
{
    Q_OBJECT

public:
    Process(QObject *parent = nullptr) : QObject(parent) {}

    void run(const QString &program, const QStringList &arguments = {});
    void jar(const QString &jar, const QStringList &arguments = {});

signals:
    void started() const;
    void finished(bool success, const QString &output) const;

protected:
    QString program;
};

#endif // PROCESS_H
