#ifndef APPLICATIONUPDATEINFO_H
#define APPLICATIONUPDATEINFO_H

#include "base/iupdateinfo.h"
#include <QWidget>

class ApplicationUpdateInfo final : public IUpdateInfo
{
public:
    ApplicationUpdateInfo(QObject *parent = nullptr) : IUpdateInfo(parent) {}

    QString getApplicationName() const override;
    QString getCurrentVersion() const override;
    QString getLatestVersion() const override;
    QString getWhatsNewUrl() const override;
    QIcon getApplicationIcon() const override;
    void download(QWidget *parent = nullptr) override;
};

#endif // APPLICATIONUPDATEINFO_H
