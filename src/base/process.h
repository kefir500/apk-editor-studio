#ifndef PROCESS_H
#define PROCESS_H

#include <QProcess>

class Process : public QObject
{
    Q_OBJECT

public:
    Process(QObject *parent = nullptr);

    void run(const QString &program, const QStringList &arguments = {});
    void jar(const QString &jar, const QStringList &arguments = {});

signals:
    void started() const;
    void finished(bool success, const QString &output) const;

protected:
    QProcess process;
};

#endif // PROCESS_H
