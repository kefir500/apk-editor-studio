#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "widgets/gradientwidget.h"

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);

private:
    GradientWidget *createAboutTab();
    QWidget *createAuthorsTab();
    QWidget *createVersionsTab();
    QWidget *createLibrariesTab();
    QWidget *createLicenseTab();
    QString createLink(const QString &url, const QString &title = QString()) const;
};

#endif // ABOUTDIALOG_H
