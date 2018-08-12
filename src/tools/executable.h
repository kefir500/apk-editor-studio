#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include "base/result.h"
#include <QProcess>

class Executable : public QObject
{
    Q_OBJECT

public:
    Executable(QObject *parent = nullptr);
    Executable(const QString &executable, QObject *parent = nullptr);
    virtual ~Executable() {}

signals:
    void success(const QString &message) const;
    void error(const QString &message) const;
    void finished() const;

protected:
    QProcess process;
    QString executable;

    virtual void startAsync(const QStringList &arguments);
    virtual Result<QString> startSync(const QStringList &arguments) const;
};

#endif // EXECUTABLE_H
