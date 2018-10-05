#ifndef DIALOGS_H
#define DIALOGS_H

#include "apk/project.h"
#include <QMessageBox>

namespace Dialogs {
    bool openApk(QWidget *parent = 0, const QString &defaultPath = QString());
    QStringList getOpenApkFilenames(QWidget *parent = 0, const QString &defaultPath = QString());
    QString getSaveApkFilename(const Project *project, QWidget *parent = 0, const QString &defaultPath = QString());

    QString getOpenImageFilename(QWidget *parent = 0, const QString &defaultPath = QString());
    QString getOpenFilename(QWidget *parent = 0, const QString &defaultPath = QString(), bool filterExtension = false);
    QStringList getOpenImageFilenames(QWidget *parent = 0, const QString &defaultPath = QString());
    QStringList getOpenFilenames(QWidget *parent = 0, const QString &defaultPath = QString(), bool filterExtension = false);
    QString getOpenDirectory(QWidget *parent = 0, const QString &defaultPath = QString());

    QString getOpenKeystoreFilename(QWidget *parent = 0, const QString &defaultPath = QString());
    QString getSaveKeystoreFilename(QWidget *parent = 0, const QString &defaultPath = QString());

    QString combo(const QStringList &options, QWidget *parent = 0);
    QString combo(const QStringList &options, const QString &current, QWidget *parent = 0);
    QString combo(const QStringList &options, const QString &current, const QString &title, QWidget *parent = 0);

    bool replaceFile(const QString &path, QWidget *parent = 0);
    bool replaceImage(const QString &path, QWidget *parent = 0);
    int detailed(const QString &text, const QString &detailed, QMessageBox::Icon icon = QMessageBox::Warning, QWidget *parent = 0);
    int log(const QString &log, QWidget *parent = 0);
}

#endif // DIALOGS_H
