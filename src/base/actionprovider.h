#ifndef ACTIONPROVIDER_H
#define ACTIONPROVIDER_H

#include <QAction>

class Project;

class ActionProvider : public QObject
{
    Q_OBJECT

public:
    ActionProvider(QObject *parent = nullptr) : QObject(parent) {}

    void openApk(QWidget *parent = nullptr);
    void openApk(const QString &path, QWidget *parent = nullptr);
    void openApk(const QStringList &paths, QWidget *parent = nullptr);
    void optimizeApk(QWidget *parent = nullptr);
    void optimizeApk(const QStringList &paths, QWidget *parent = nullptr);
    void signApk(QWidget *parent = nullptr);
    void signApk(const QStringList &paths, QWidget *parent = nullptr);
    void signApk(const QStringList &paths, const Keystore *keystore, QWidget *parent = nullptr);
    void installApk(QWidget *parent = nullptr);
    void installApk(const QString &serial, QWidget *parent = nullptr);
    void installApk(const QStringList &paths, const QString &serial, QWidget *parent = nullptr);
    bool closeApk(Project *project);

    void visitWebPage();
    void visitSourcePage();
    void visitDonatePage();
    void visitUpdatePage();
    void visitBlogPage(const QString &post);
    void exit();

    void addToRecent(const Project *project) const;
    void checkUpdates(QWidget *parent = nullptr) const;
    bool resetSettings(QWidget *parent = nullptr) const;

    void openOptions(QWidget *parent = nullptr);
    void openDeviceManager(QWidget *parent = nullptr);
    void openKeyManager(QWidget *parent = nullptr);
    void openAndroidExplorer(QWidget *parent = nullptr);
    void openAndroidExplorer(const QString &serial, QWidget *parent = nullptr);
    void takeScreenshot(QWidget *parent = nullptr);
    void takeScreenshot(const QString &serial, QWidget *parent = nullptr);

    QAction *getOpenApk(QWidget *parent = nullptr);
    QAction *getOptimizeApk(QWidget *parent = nullptr);
    QAction *getSignApk(QWidget *parent = nullptr);
    QAction *getInstallApk(QWidget *parent = nullptr);
    QAction *getInstallApk(const QString &serial, QWidget *parent = nullptr);

    QAction *getVisitWebPage(QObject *parent = nullptr);
    QAction *getVisitSourcePage(QObject *parent = nullptr);
    QAction *getVisitDonatePage(QObject *parent = nullptr);
    QAction *getExit(QObject *parent = nullptr);

    QAction *getCheckUpdates(QWidget *parent = nullptr);
    QAction *getResetSettings(QWidget *parent = nullptr);
    QAction *getOpenOptions(QWidget *parent = nullptr);
    QAction *getOpenDeviceManager(QWidget *parent= nullptr);
    QAction *getOpenKeyManager(QWidget *parent = nullptr);
    QAction *getOpenAndroidExplorer(QWidget *parent = nullptr);
    QAction *getOpenAndroidExplorer(const QString &serial, QWidget *parent = nullptr);
    QAction *getTakeScreenshot(QWidget *parent = nullptr);
    QAction *getTakeScreenshot(const QString &serial, QWidget *parent = nullptr);

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
