#include "base/applicationupdateinfo.h"
#include "base/application.h"
#include "base/utils.h"
#include <QJsonDocument>
#include <QJsonObject>

QString ApplicationUpdateInfo::getApplicationName() const
{
    return APPLICATION;
}

QString ApplicationUpdateInfo::getCurrentVersion() const
{
    return VERSION;
}

QString ApplicationUpdateInfo::getLatestVersion() const
{
#if defined(Q_OS_WIN)
    const QString os = "windows";
#elif defined(Q_OS_MACOS)
    const QString os = "macos";
#elif defined(Q_OS_LINUX)
    const QString os = "linux";
#else
    const QString os = "windows";
#endif
    return QJsonDocument::fromJson(metadata)
                          .object()["os"]
                          .toObject()[os]
                          .toObject()["version"]
                          .toString();
}

QString ApplicationUpdateInfo::getWhatsNewUrl() const
{
    const auto latestVersion = getLatestVersion();
    if (latestVersion.isEmpty()) {
        return {};
    }
    return Utils::getBlogPostUrl(QString("apk-editor-studio-v%1-released").arg(getLatestVersion()));
}

QIcon ApplicationUpdateInfo::getApplicationIcon() const
{
    return QIcon::fromTheme("apk-editor-studio");
}

void ApplicationUpdateInfo::download(QWidget *parent)
{
    Q_UNUSED(parent)
    app->actions.visitUpdatePage();
}
