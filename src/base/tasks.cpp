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
    auto apktool = new Apktool::Decode(source, target, frameworks, resources, sources, keepBroken, this);
    connect(apktool, &Apktool::Decode::finished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        apktool->deleteLater();
    });
    apktool->run();
}

// Pack

Pack::Pack(const QString &source, const QString &target, const QString &frameworks, bool aapt2)
{
    this->source = source;
    this->target = target;
    this->frameworks = frameworks;
    this->aapt2 = aapt2;
}

void Pack::run()
{
    emit started();
    auto apktool = new Apktool::Build(source, target, frameworks, aapt2, this);
    connect(apktool, &Apktool::Build::finished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        apktool->deleteLater();
    });
    apktool->run();
}

// Zipalign

Align::Align(const QString &target)
{
    this->target = target;
}

void Align::run()
{
    emit started();
    auto zipalign = new Zipalign::Align(target, this);
    connect(zipalign, &Zipalign::Align::finished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        zipalign->deleteLater();
    });
    zipalign->run();
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
    auto apksigner = new Apksigner::Sign(target, keystore, this);
    connect(apksigner, &Apksigner::Sign::finished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        apksigner->deleteLater();
    });
    apksigner->run();
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
    auto adb = new Adb::Install(apk, serial, this);
    connect(adb, &Adb::Install::finished, [=](bool ok, const QString &message) {
        emit ok ? success() : error(message);
        emit finished();
        adb->deleteLater();
    });
    adb->run();
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
