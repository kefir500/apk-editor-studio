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
    void exit();

    void checkUpdates(QWidget *parent);
    bool installExternalApk(QWidget *parent = nullptr);

    void openOptions(QWidget *parent = nullptr);
    void openDeviceManager(QWidget *parent = nullptr);
    void openKeyManager(QWidget *parent = nullptr);
    void openAndroidExplorer(QWidget *parent = nullptr);

    void takeScreenshot(QWidget *parent = nullptr);

    QAction *getOpenApk(QWidget *parent = nullptr);

    QAction *getVisitWebPage(QObject *parent = nullptr);
    QAction *getVisitSourcePage(QObject *parent = nullptr);
    QAction *getVisitDonatePage(QObject *parent = nullptr);
    QAction *getExit(QObject *parent = nullptr);

    QAction *getCheckUpdates(QWidget *parent = nullptr);
    QAction *getInstallExternalApk(QWidget *parent = nullptr);
    QAction *getOpenOptions(QWidget *parent = nullptr);
    QAction *getOpenDeviceManager(QWidget *parent= nullptr);
    QAction *getOpenKeyManager(QWidget *parent = nullptr);
    QAction *getOpenAndroidExplorer(QWidget *parent = nullptr);

    QAction *getTakeScreenshot(QWidget *parent = nullptr);

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
