#ifndef APKTOOL_H
#define APKTOOL_H

#include "tools/command.h"

namespace Apktool
{
    class Decode : public Command
    {
        Q_OBJECT

    public:
        Decode(const QString &source, const QString &destination, const QString &frameworks,
               bool resources, bool sources, bool keepBroken, QObject *parent = nullptr)
            : Command(parent)
            , source(source)
            , destination(destination)
            , frameworks(frameworks)
            , resources(resources)
            , sources(sources)
            , keepBroken(keepBroken) {}

        void run() override;

    private:
        const QString source;
        const QString destination;
        const QString frameworks;
        const bool resources;
        const bool sources;
        const bool keepBroken;

    signals:
        void finished(bool success, const QString &output) const;
    };

    class Build : public Command
    {
        Q_OBJECT

    public:
        Build(const QString &source, const QString &destination,
              const QString &frameworks, QObject *parent = nullptr)
            : Command(parent)
            , source(source)
            , destination(destination)
            , frameworks(frameworks) {}

        void run() override;

    private:
        const QString source;
        const QString destination;
        const QString frameworks;

    signals:
        void finished(bool success, const QString &output) const;
    };

    class Version : public Command
    {
        Q_OBJECT

    public:
        Version(QObject *parent = nullptr) : Command(parent) {}
        void run() override;

    signals:
        void finished(const QString &version) const;
    };

    void reset();
    QString getPath();
    QString getDefaultPath();
    QString getOutputPath();
    QString getDefaultOutputPath();
    QString getFrameworksPath();
    QString getDefaultFrameworksPath();
}

#endif // APKTOOL_H
