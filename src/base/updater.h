#ifndef UPDATER_H
#define UPDATER_H

#include <QCoreApplication>

class Updater final
{
    Q_DECLARE_TR_FUNCTIONS(Updater)

public:
    Updater() = delete;

    static void check(bool verbose = false, QWidget *parent = nullptr);
    static void download();
    static void whatsnew(const QString &version);

private:
    static QString parse(const QByteArray &json);
    static bool compare(const QString &currentVersion, const QString &latestVersion);
    static void notify(const QString &version, QWidget *parent = nullptr);
};

#endif // UPDATER_H
