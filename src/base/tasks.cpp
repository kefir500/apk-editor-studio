#include "base/tasks.h"
#include "tools/apktool.h"
#include "tools/apksigner.h"
#include "tools/zipalign.h"
#include "tools/adb.h"
#include <QThreadPool>

using namespace Tasks;

// Task

Task::Task()
{
    connect(this, &Task::finished, this, &Task::deleteLater);
}

// Unpack

Unpack::Unpack(const QString &source, const QString &target, const QString &frameworks, bool resources, bool sources)
{
    this->source = source;
    this->target = target;
    this->frameworks = frameworks;
    this->resources = resources;
    this->sources = sources;
}

void Unpack::run()
{
    emit started();
    Apktool *apktool = new Apktool(this);
    connect(apktool, &Executable::success, this, &Task::success);
    connect(apktool, &Executable::error, this, &Task::error);
    connect(apktool, &Executable::finished, this, &Task::finished);
    connect(apktool, &Executable::finished, apktool, &QObject::deleteLater);
    apktool->decode(source, target, frameworks, resources, sources);
}

// Pack

Pack::Pack(const QString &source, const QString &target, const QString &frameworks, bool resources, bool sources)
{
    this->source = source;
    this->target = target;
    this->frameworks = frameworks;
    this->resources = resources;
    this->sources = sources;
}

void Pack::run()
{
    emit started();
    Apktool *apktool = new Apktool(this);
    connect(apktool, &Executable::success, this, &Task::success);
    connect(apktool, &Executable::error, this, &Task::error);
    connect(apktool, &Executable::finished, this, &Task::finished);
    connect(apktool, &Executable::finished, apktool, &QObject::deleteLater);
    apktool->build(source, target, frameworks, resources, sources);
}

// Zipalign

Align::Align(const QString &target)
{
    this->target = target;
}

void Align::run()
{
    emit started();
    Zipalign *zipalign = new Zipalign(this);
    connect(zipalign, &Executable::success, this, &Task::success);
    connect(zipalign, &Executable::error, this, &Task::error);
    connect(zipalign, &Executable::finished, this, &Task::finished);
    connect(zipalign, &Executable::finished, zipalign, &QObject::deleteLater);
    zipalign->align(target);
}

// Sign

Sign::Sign(const QString &target, const Keystore *keystore)
{
    this->target = target;
    this->keystore = keystore;
}

void Sign::run()
{
    emit started();
    Apksigner *apksigner = new Apksigner(this);
    connect(apksigner, &Executable::success, this, &Task::success);
    connect(apksigner, &Executable::error, this, &Task::error);
    connect(apksigner, &Executable::finished, this, &Task::finished);
    connect(apksigner, &Executable::finished, apksigner, &QObject::deleteLater);
    apksigner->sign(target, keystore);
}

// Install

Install::Install(const QString &apk, const QString &serial)
{
    this->apk = apk;
    this->serial = serial;
}

void Install::run()
{
    emit started();
    Adb *adb = new Adb(this);
    connect(adb, &Executable::success, this, &Task::success);
    connect(adb, &Executable::error, this, &Task::error);
    connect(adb, &Executable::finished, this, &Task::finished);
    connect(adb, &Executable::finished, adb, &QObject::deleteLater);
    adb->install(apk, serial);
}

// Batch

Batch::~Batch()
{
    auto it = tasks.begin();
    auto end = tasks.end();
    while (it != end) {
        delete *it;
        ++it;
    }
}

void Batch::add(Task *task, bool critical)
{
    tasks.enqueue(task);
    if (critical) {
        QObject::connect(task, &Task::error, [=](const QString &message) {
            emit error(message);
            emit finished();
            delete this;
        });
    }
    QObject::connect(task, &Task::finished, this, &Batch::run);
}

void Batch::run()
{
    emit started();
    if (!tasks.isEmpty()) {
        Task *task = tasks.dequeue();
        task->run();
    } else {
        emit success();
        emit finished();
        delete this;
    }
}
