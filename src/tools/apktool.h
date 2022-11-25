#ifndef APKTOOL_H
#define APKTOOL_H

#include "base/command.h"

namespace Apktool
{
    class Decode : public Command
    {
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
        const QString &output() const;

    private:
        const QString source;
        const QString destination;
        const QString frameworks;
        const bool resources;
        const bool sources;
        const bool keepBroken;
        QString resultOutput;
    };

    class Build : public Command
    {
    public:
        Build(const QString &source, const QString &destination,
              const QString &frameworks, bool aapt2, bool debuggable, QObject *parent = nullptr)
            : Command(parent)
            , source(source)
            , destination(destination)
            , frameworks(frameworks)
            , aapt2(aapt2)
            , debuggable(debuggable)
        {}

        void run() override;
        const QString &output() const;

    private:
        const QString source;
        const QString destination;
        const QString frameworks;
        const bool aapt2;
        const bool debuggable;
        QString resultOutput;
    };

    class InstallFramework : public Command
    {
    public:
        InstallFramework(const QString &source, const QString &destination, QObject *parent = nullptr)
            : Command(parent)
            , source(source)
            , destination(destination)
        {}

        void run() override;
        const QString &output() const;

    private:
        const QString source;
        const QString destination;
        QString resultOutput;
    };

    class Version : public Command
    {
    public:
        Version(QObject *parent = nullptr) : Command(parent) {}
        void run() override;
        const QString &version() const;

    private:
        QString resultVersion;
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
