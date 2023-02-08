#include "base/apktoolupdateinfo.h"
#include "windows/downloader.h"
#include "base/application.h"
#include "base/settings.h"
#include "base/utils.h"
#include "tools/apktool.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

ApktoolUpdateInfo::ApktoolUpdateInfo(QObject *parent) : IUpdateInfo(parent)
{
    refresh();
}

QString ApktoolUpdateInfo::getApplicationName() const
{
    return "Apktool";
}

QString ApktoolUpdateInfo::getCurrentVersion() const
{
    return currentVersion;
}

QString ApktoolUpdateInfo::getLatestVersion() const
{
    return QJsonDocument::fromJson(metadata)
                          .object()["apktool"]
                          .toObject()["version"]
                          .toString();
}

QString ApktoolUpdateInfo::getWhatsNewUrl() const
{
    return QJsonDocument::fromJson(metadata)
                          .object()["apktool"]
                          .toObject()["changelog"]
                          .toString();
}

QIcon ApktoolUpdateInfo::getApplicationIcon() const
{
    return QIcon::fromTheme("apktool");
}

void ApktoolUpdateInfo::download(QWidget *parent)
{
    const auto downloadUrl = QJsonDocument::fromJson(metadata)
                                            .object()["apktool"]
                                            .toObject()["download"]
                                            .toString();

    const QString outputPath = Utils::getLocalConfigPath("apktool");

    Downloader downloader(QString("Apktool v%1").arg(getLatestVersion()), downloadUrl, outputPath, parent);
    connect(&downloader, &Downloader::success, this, [this](const QString &path) {
#ifndef PORTABLE
        const QString newApktoolPath = path;
#else
        const QString newApktoolPath = QDir(app->applicationDirPath()).relativeFilePath(path);
#endif
        app->settings->setApktoolPath(newApktoolPath);
        refresh();
        Apktool::reset();
    });
    downloader.exec();
}

void ApktoolUpdateInfo::refresh()
{
    currentVersion.clear();

    auto apktool = new Apktool::Version(this);
    connect(apktool, &Apktool::Version::finished, this, [=](bool success) {
        currentVersion = apktool->version();
        emit updated();
        apktool->deleteLater();
    });
    apktool->run();
}

bool ApktoolUpdateInfo::hasUpdates() const
{
    if (getCurrentVersion().isEmpty() && QFile::exists(Apktool::getPath())) {
        return false;
    }

    return IUpdateInfo::hasUpdates();
}
