#ifndef DIALOGS_H
#define DIALOGS_H

#include "apk/project.h"
#include <QMessageBox>

namespace Dialogs
{
    bool openApk(QWidget *parent = nullptr, const QString &defaultPath = QString());
    QStringList getOpenApkFilenames(QWidget *parent = nullptr, const QString &defaultPath = QString());
    QString getSaveApkFilename(const Project *project, QWidget *parent = nullptr, const QString &defaultPath = QString());

    QString getOpenImageFilename(QWidget *parent = nullptr, const QString &defaultPath = QString());
    QString getOpenFilename(QWidget *parent = nullptr, const QString &defaultPath = QString(), bool filterExtension = false);
    QStringList getOpenImageFilenames(QWidget *parent = nullptr, const QString &defaultPath = QString());
    QStringList getOpenFilenames(QWidget *parent = nullptr, const QString &defaultPath = QString(), bool filterExtension = false);
    QString getOpenDirectory(QWidget *parent = nullptr, const QString &defaultPath = QString());

    QString getOpenKeystoreFilename(QWidget *parent = nullptr, const QString &defaultPath = QString());
    QString getSaveKeystoreFilename(QWidget *parent = nullptr, const QString &defaultPath = QString());

    QString combo(const QStringList &options, QWidget *parent = nullptr);
    QString combo(const QStringList &options, const QString &current, QWidget *parent = nullptr);
    QString combo(const QStringList &options, const QString &current, const QString &title, QWidget *parent = nullptr);

    bool replaceFile(const QString &path, QWidget *parent = nullptr);
    bool replaceImage(const QString &path, QWidget *parent = nullptr);
    int detailed(const QString &text, const QString &detailed, QMessageBox::Icon icon = QMessageBox::Warning, QWidget *parent = nullptr);
    int log(const QString &log, QWidget *parent = nullptr);
}

#endif // DIALOGS_H
