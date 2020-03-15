#ifndef ACTIONPROVIDER_H
#define ACTIONPROVIDER_H

#include <QAction>

class Project;

class ActionProvider : public QObject
{
    Q_OBJECT

public:
    ActionProvider(QObject *parent = nullptr) : QObject(parent) {}

    bool openApk(QWidget *parent = nullptr);
    bool openApk(const QString &defaultPath, QWidget *parent = nullptr);
    bool closeApk(Project *project);

    void visitWebPage();
    void visitSourcePage();
    void visitDonatePage();
    void visitUpdatePage();
    void visitBlogPage(const QString &post);
    bool associateApk();
    void exit();

    void checkUpdates(QWidget *parent = nullptr);
    bool resetSettings(QWidget *parent = nullptr);
    bool installExternalApk(const QString &path, QString serial, QWidget *parent = nullptr);
    bool installExternalApks(QStringList paths, QString serial, QWidget *parent = nullptr);

    void openOptions(QWidget *parent = nullptr);
    void openDeviceManager(QWidget *parent = nullptr);
    void openKeyManager(QWidget *parent = nullptr);
    bool openAndroidExplorer(QWidget *parent = nullptr);
    bool openAndroidExplorer(const QString &serial = QString(), QWidget *parent = nullptr);
    bool takeScreenshot(QWidget *parent = nullptr);
    bool takeScreenshot(const QString &serial = QString(), QWidget *parent = nullptr);

    QAction *getOpenApk(QWidget *parent = nullptr);

    QAction *getVisitWebPage(QObject *parent = nullptr);
    QAction *getVisitSourcePage(QObject *parent = nullptr);
    QAction *getVisitDonatePage(QObject *parent = nullptr);
    QAction *getExit(QObject *parent = nullptr);

    QAction *getCheckUpdates(QWidget *parent = nullptr);
    QAction *getResetSettings(QWidget *parent = nullptr);
    QAction *getInstallExternalApk(QWidget *parent = nullptr);
    QAction *getInstallExternalApk(const QString &serial = QString(), QWidget *parent = nullptr);
    QAction *getOpenOptions(QWidget *parent = nullptr);
    QAction *getOpenDeviceManager(QWidget *parent= nullptr);
    QAction *getOpenKeyManager(QWidget *parent = nullptr);
    QAction *getOpenAndroidExplorer(QWidget *parent = nullptr);
    QAction *getOpenAndroidExplorer(const QString &serial = QString(), QWidget *parent = nullptr);
    QAction *getTakeScreenshot(QWidget *parent = nullptr);
    QAction *getTakeScreenshot(const QString &serial = QString(), QWidget *parent = nullptr);

    QMenu *getLanguages(QWidget *parent = nullptr);
    QMenu *getRecent(QWidget *parent = nullptr);

    bool event(QEvent *event) override;

signals:
    void languageChanged();

private:
    QAction *getClearRecent(QObject *parent = nullptr);
    QAction *getNoRecent(QObject *parent = nullptr);
};

#endif // ACTIONPROVIDER_H
