#ifndef UPDATER_H
#define UPDATER_H

#include <QtNetwork/QNetworkAccessManager>

class Updater : public QObject {

public:
    explicit Updater(bool verbose = false, QWidget *parent = 0);
    void checkAndDelete();
    void download() const;

private:
    QString parse(const QByteArray &json) const;
    bool compare(const QString &currentVersion, const QString &latestVersion) const;
    void notify(const QString &version) const;

    QNetworkAccessManager *http;
    bool verbose;
};

#endif // UPDATER_H
