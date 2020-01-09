#include "base/updater.h"
#include "base/application.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QMessageBox>
#include <QDebug>

void Updater::check(bool verbose, QWidget *parent)
{
    auto http = new QNetworkAccessManager;

    QObject::connect(http, &QNetworkAccessManager::finished, parent, [=](QNetworkReply *reply) {
        QNetworkReply::NetworkError error = reply->error();
        if (error == QNetworkReply::NoError) {
            const QString currentVersion = VERSION;
            const QString latestVersion = parse(reply->readAll());
            if (!latestVersion.isEmpty()) {
                if (compare(currentVersion, latestVersion)) {
                    qDebug() << qPrintable(QString("Updater: New version available (%1).\n").arg(latestVersion));
                    notify(latestVersion, parent);
                } else {
                    if (verbose) {
                        //: Don't translate the "APK Editor Studio" part.
                        QMessageBox::information(parent, QString(), tr("You are using the latest version of APK Editor Studio."));
                    }
                }
            } else {
                qWarning() << "Updater: Could not parse JSON.\n";
                if (verbose) {
                    QMessageBox::warning(parent, QString(), tr("Could not check for updates: invalid format."));
                }
            }
        } else {
            qDebug() << qPrintable(QString("Updater: Could not check for updates (%1).\n").arg(error));
            if (verbose) {
                //: %1 will be replaced with an error code.
                QMessageBox::warning(parent, QString(), tr("Could not check for updates (error %1).").arg(error));
            }
        }
        reply->deleteLater();
        http->deleteLater();
    }, Qt::QueuedConnection);

    auto thread = new QThread();
    http->moveToThread(thread);
    QObject::connect(http, &QObject::destroyed, thread, &QThread::quit);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    QObject::connect(thread, &QThread::started, [=]() {
        http->get(QNetworkRequest(app->getUpdateUrl()));
    });
    thread->start();
}

void Updater::download()
{
    app->visitUpdatePage();
}

void Updater::whatsnew(const QString &version)
{
    app->visitBlogPage(QString("apk-editor-studio-v%1-released").arg(version));
}

QString Updater::parse(const QByteArray &json)
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
                          .object()["os"]
                          .toObject()[os]
                          .toObject()["version"].toString();
}

bool Updater::compare(const QString &currentVersion, const QString &latestVersion)
{
    QStringList currentVersionSegments = currentVersion.split('.');
    QStringList latestVersionSegments = latestVersion.split('.');
    const int currentVersionSegmentCount = currentVersionSegments.size();
    const int latestVersionSegmentCount = latestVersionSegments.size();

    // Normalize segments count:
    const int minSegments = qMin(currentVersionSegmentCount, latestVersionSegmentCount);
    const int maxSegments = qMax(currentVersionSegmentCount, latestVersionSegmentCount);
    for (int i = minSegments; i < maxSegments; ++i) {
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

void Updater::notify(const QString &version, QWidget *parent)
{
    QMessageBox question(parent);
    //: This is a noun.
    question.setWindowTitle(tr("Update"));
    //: "v%1" will be replaced with a software version (e.g., v1.0.0, v2.1.2...). Also, don't translate the "APK Editor Studio" part.
    question.setText(tr("APK Editor Studio v%1 is available. Download?").arg(version));
    question.setIcon(QMessageBox::Question);
    question.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    auto btnWhatsNew = question.addButton(tr("What's New"), QMessageBox::ActionRole);
    QObject::connect(btnWhatsNew, &QPushButton::clicked, [=]() {
        whatsnew(version);
    });
    if (question.exec() == QMessageBox::Yes) {
        download();
    }
}
