#ifndef APKTOOL_H
#define APKTOOL_H

#include <QObject>

class Apktool : public QObject
{
    Q_OBJECT

public:
    explicit Apktool(QObject *parent = nullptr) : QObject(parent) {}

    void decode(const QString &source, const QString &destination, const QString &frameworks, bool resources, bool sources, bool keepBroken);
    void build(const QString &source, const QString &destination, const QString &frameworks);
    void version();
    void reset();

    static QString getPath();
    static QString getDefaultPath();
    static QString getOutputPath();
    static QString getDefaultOutputPath();
    static QString getFrameworksPath();
    static QString getDefaultFrameworksPath();

signals:
    void decodeFinished(bool success, const QString &message = QString()) const;
    void buildFinished(bool success, const QString &message = QString()) const;
    void versionFetched(const QString &version) const;
    void resetFinished() const;
};

#endif // APKTOOL_H
