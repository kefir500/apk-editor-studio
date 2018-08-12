#include "base/updater.h"
#include "base/application.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QDebug>

Updater::Updater(bool verbose, QWidget *parent) : QObject(parent)
{
    http = new QNetworkAccessManager(this);

    tr("What's new:"); // TODO For future use
    connect(http, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            const QString currentVersion = VERSION;
            const QString latestVersion = parse(reply->readAll());
            if (!latestVersion.isEmpty()) {
                if (compare(currentVersion, latestVersion)) {
                    qDebug() << qPrintable(QString("Updater: New version avaiable (%1).\n").arg(latestVersion));
                    notify(latestVersion);
                } else {
                    if (verbose) {
                        QMessageBox::information(parent, QString(), tr("You are using the latest version of %1."));
                    }
                }
            } else {
                qDebug() << "Updater: Could not parse JSON.\n";
                if (verbose) {
                    QMessageBox::warning(parent, QString(), tr("Could not check for updates: invalid format."));
                }
            }
        } else {
            qDebug() << qPrintable(QString("Updater: Could not check for updates (%1).\n").arg(error));
            if (verbose) {
                QMessageBox::warning(parent, QString(), tr("Could not check for updates (error %1).").arg(error));
            }
        }
        reply->deleteLater();
        deleteLater();
    });
}

void Updater::checkAndDelete()
{
    http->get(QNetworkRequest(app->getUpdateUrl()));
}

void Updater::download() const
{
    app->visitWebPage();
}

QString Updater::parse(const QByteArray &json) const
{
#if defined(Q_OS_WIN)
    const QString os = "windows";
#elif defined(Q_OS_OSX)
    const QString os = "macos";
#elif defined(Q_OS_LINUX)
    const QString os = "linux";
#else
    const QString os = "windows";
#endif
    return QJsonDocument::fromJson(json)
                          .object()["1"]
                          .toObject()[os]
                          .toObject()["version"].toString();
}

bool Updater::compare(const QString &currentVersion, const QString &latestVersion) const
{
    QStringList currentVersionSegments = currentVersion.split('.');
    QStringList latestVersionSegments = latestVersion.split('.');
    const int currentVersionSegmentCount = currentVersionSegments.size();
    const int latestVersionSegmentCount = latestVersionSegments.size();

    // Normalize segments count:
    const int minSegments = qMin(currentVersionSegmentCount, latestVersionSegmentCount);
    const int maxSegments = qMax(currentVersionSegmentCount, latestVersionSegmentCount);
    for (short i = minSegments; i < maxSegments; ++i) {
        if (currentVersionSegments.size() <= i) {
            currentVersionSegments.append("0");
        } else if (latestVersionSegments.size() <= i) {
            latestVersionSegments.append("0");
        }
    }

    // Compare segments:
    for (short i = 0; i < maxSegments; ++i) {
        const int currentVersionSegment = currentVersionSegments.at(i).toInt();
        const int latestVersionSegment = latestVersionSegments.at(i).toInt();
        if (currentVersionSegment == latestVersionSegment) {
            continue;
        } else {
            return (latestVersionSegment > currentVersionSegment);
        }
    }
    return false;
}

void Updater::notify(const QString &version) const
{
    QWidget *parentWidget = static_cast<QWidget *>(parent());
    const int answer = QMessageBox::question(parentWidget, tr("Update"), tr("%1 v%2 is avaiable.\nDownload?").arg(APPLICATION, version));
    if (answer == QMessageBox::Yes) {
        download();
    }
}
