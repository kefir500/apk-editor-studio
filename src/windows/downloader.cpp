#include "windows/downloader.h"
#include "base/utils.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkReply>
#include <QProgressBar>

Downloader::Downloader(const QString &title, const QUrl &downloadUrl, const QString &outputDirectory, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Downloading"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    //: "%1" will be replaced with a title of the downloaded file.
    auto description = new QLabel(tr("Downloading %1...").arg(title), this);

    auto progress = new QProgressBar(this);
    progress->setValue(0);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &Downloader::reject);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(description);
    layout->addWidget(progress);
    layout->addWidget(buttons);

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
                QMessageBox::warning(parent, {}, QString("%1\n%2").arg(tr("Could not save %1:").arg(title), output.errorString()));
            }
        } else {
            //: "%1" will be replaced with a title of the downloaded file.
            QMessageBox::warning(parent, {}, QString("%1\n%2").arg(tr("Could not download %1:").arg(title), reply->errorString()));
        }
        reply->deleteLater();
        close();
    });

    connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal) {
        progress->setMaximum(bytesTotal);
        progress->setValue(bytesReceived);
    });

    adjustSize();
    resize(Utils::scale(400, 0));
}
