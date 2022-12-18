#ifndef APKCLONER_H
#define APKCLONER_H

#include <QObject>

class ApkCloner : public QObject
{
    Q_OBJECT

public:
    explicit ApkCloner(const QString &contentsPath, const QString &originalPackageName,
                       const QString &newPackageName, QObject *parent = nullptr);

    void start();

signals:
    void started();
    void progressed(const QString &stage, const QString &filename);
    void finished(bool success);

private:
    QString contentsPath;
    QString originalPackageName;
    QString originalPackagePath;
    QString newPackageName;
    QString newPackagePath;
};

#endif // APKCLONER_H
