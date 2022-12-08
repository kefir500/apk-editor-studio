#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QDialog>
#include <QNetworkAccessManager>

class Downloader : public QDialog
{
    Q_OBJECT

public:
    explicit Downloader(const QString &title, const QUrl &downloadUrl, const QString &outputPath, QWidget *parent = nullptr);

signals:
    void success(const QString &path);
    void error(const QString &error);

private:
    QNetworkAccessManager http;
};

#endif // DOWNLOADER_H
