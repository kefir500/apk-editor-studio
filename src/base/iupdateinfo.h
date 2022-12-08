#ifndef IUPDATEINFO_H
#define IUPDATEINFO_H

#include <QObject>

class IUpdateInfo : public QObject
{
    Q_OBJECT

public:
    IUpdateInfo(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IUpdateInfo() = default;

    virtual QString getApplicationName() const = 0;
    virtual QString getCurrentVersion() const = 0;
    virtual QString getLatestVersion() const = 0;
    virtual QString getWhatsNewUrl() const = 0;
    virtual QIcon getApplicationIcon() const = 0;
    virtual void download(QWidget *parent = nullptr) = 0;
    virtual void refresh() {};

    bool hasUpdates() const;
    void setMetadata(const QByteArray &metadata);

signals:
    void updated();

protected:
    QByteArray metadata;

private:
    bool isNewerVersion(const QString &currentVersion, const QString &latestVersion) const;
};

#endif // IUPDATEINFO_H
