#ifndef PROCESS_H
#define PROCESS_H

#include <QProcess>

class Process : public QObject
{
    Q_OBJECT

public:
    Process(QObject *parent = nullptr);

    virtual void run(const QString &program, const QStringList &arguments = {});

    void setStandardOutputFile(const QString &filename);

signals:
    void started();
    void finished(bool success, const QString &output);

protected:
    QProcess process;
};

#endif // PROCESS_H
