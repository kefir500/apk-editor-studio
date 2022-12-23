#include "windows/downloader.h"
#include "base/utils.h"
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QNetworkReply>

Downloader::Downloader(const QString &title, const QUrl &downloadUrl, const QString &outputDirectory, QWidget *parent)
    : ProgressDialog(parent)
{
    setWindowTitle(tr("Downloading"));

    //: "%1" will be replaced with a title of the downloaded file.
    setPrimaryText(tr("Downloading %1...").arg(title));
    setProgressMaximum(1);

    QNetworkRequest request;
    request.setUrl(downloadUrl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    auto reply = http.get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QDir().mkpath(outputDirectory);
            const QString outputPath = QString("%1/%2").arg(outputDirectory, downloadUrl.fileName());
            QFile output(outputPath);
            if (output.open(QFile::WriteOnly | QFile::Truncate)) {
                output.write(reply->readAll());
                output.close();
                emit success(outputPath);
            } else {
                //: "%1" will be replaced with a title of the saved file.
                const QString message(tr("Could not save %1:"));
                QMessageBox::warning(parent, {}, QString("%1\n%2").arg(message.arg(title), output.errorString()));
            }
        } else {
            //: "%1" will be replaced with a title of the downloaded file.
            const QString message(tr("Could not download %1:"));
            QMessageBox::warning(parent, {}, QString("%1\n%2").arg(message.arg(title), reply->errorString()));
        }
        reply->deleteLater();
        close();
    });

    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal) {
        setProgressMaximum(bytesTotal);
        setProgressValue(bytesReceived);
    });

    adjustSize();
    resize(Utils::scale(400, 0));
}
