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

Unpack::Unpack(const QString &source, const QString &target, const QString &frameworks, bool resources, bool sources, bool keepBroken)
{
    this->source = source;
    this->target = target;
    this->frameworks = frameworks;
    this->resources = resources;
    this->sources = sources;
    this->keepBroken = keepBroken;
}

void Unpack::run()
{
    emit started();
    Apktool *apktool = new Apktool(this);
    connect(apktool, &Apktool::decodeFinished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        apktool->deleteLater();
    });
    apktool->decode(source, target, frameworks, resources, sources, keepBroken);
}

// Pack

Pack::Pack(const QString &source, const QString &target, const QString &frameworks)
{
    this->source = source;
    this->target = target;
    this->frameworks = frameworks;
}

void Pack::run()
{
    emit started();
    Apktool *apktool = new Apktool(this);
    connect(apktool, &Apktool::buildFinished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        apktool->deleteLater();
    });
    apktool->build(source, target, frameworks);
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
    connect(zipalign, &Zipalign::alignFinished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        zipalign->deleteLater();
    });
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
    connect(apksigner, &Apksigner::signFinished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        apksigner->deleteLater();
    });
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
    connect(adb, &Adb::installFinished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        adb->deleteLater();
    });
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
