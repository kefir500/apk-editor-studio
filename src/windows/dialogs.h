#ifndef DIALOGS_H
#define DIALOGS_H

#include "apk/project.h"
#include "base/fileformatlist.h"
#include "base/device.h"
#include <QMessageBox>

namespace Dialogs
{
    QString getOpenFilename(QWidget *parent = nullptr);
    QString getOpenFilename(const QString &defaultPath, QWidget *parent = nullptr);
    QString getOpenFilename(const QString &defaultPath, const FileFormatList &formats, QWidget *parent = nullptr);
    QString getSaveFilename(const QString &defaultPath, QWidget *parent = nullptr);
    QString getSaveFilename(const QString &defaultPath, const FileFormatList &formats, QWidget *parent = nullptr);
    QStringList getOpenFilenames(const QString &defaultPath, const FileFormatList &formats, QWidget *parent = nullptr);

    QString getOpenImageFilename(QWidget *parent = nullptr);
    QString getOpenImageFilename(const QString &defaultPath, QWidget *parent = nullptr);
    QString getSaveImageFilename(const QString &defaultPath, QWidget *parent = nullptr);
    QStringList getOpenImageFilenames(const QString &defaultPath, QWidget *parent = nullptr);

    QStringList getOpenApkFilenames(QWidget *parent = nullptr);
    QString getSaveApkFilename(const Project *project, QWidget *parent = nullptr);

    QString getOpenKeystoreFilename(const QString &defaultPath = QString(), QWidget *parent = nullptr);
    QString getSaveKeystoreFilename(const QString &defaultPath = QString(), QWidget *parent = nullptr);

    QString getOpenDirectory(const QString &defaultPath, QWidget *parent = nullptr);

    Device getInstallDevice(QWidget *parent = nullptr);
    Device getExplorerDevice(QWidget *parent = nullptr);
    Device getScreenshotDevice(QWidget *parent = nullptr);

    int detailed(const QString &text, const QString &detailed, QMessageBox::Icon icon = QMessageBox::Warning, QWidget *parent = nullptr);
    int log(const QString &title, const QString &text, QWidget *parent = nullptr);
    int log(const QString &text, QWidget *parent = nullptr);
}

#endif // DIALOGS_H
