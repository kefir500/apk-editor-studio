#ifndef ACTIONPROVIDER_H
#define ACTIONPROVIDER_H

#include <QAction>

class Project;
class ProjectItemsModel;
class Keystore;
class MainWindow;

class ActionProvider : public QObject
{
    Q_OBJECT

public:
    ActionProvider(ProjectItemsModel &projects, QObject *parent = nullptr)
        : QObject(parent)
        , projects(projects) {}

    void openApk(MainWindow *window = nullptr) const;
    void openApk(const QString &path, MainWindow *window = nullptr) const;
    void openApk(const QStringList &paths, MainWindow *window = nullptr) const;
    void optimizeApk(MainWindow *window = nullptr) const;
    void optimizeApk(const QStringList &paths, MainWindow *window = nullptr) const;
    void signApk(MainWindow *window = nullptr) const;
    void signApk(const QStringList &paths, MainWindow *window = nullptr) const;
    void signApk(const QStringList &paths, const Keystore *keystore, MainWindow *window = nullptr) const;
    void installApk(MainWindow *window = nullptr) const;
    void installApk(const QString &serial, MainWindow *window = nullptr) const;
    void installApk(const QStringList &paths, const QString &serial, MainWindow *window = nullptr) const;
    bool closeApk(Project *project) const;

    void visitWebPage() const;
    void visitSourcePage() const;
    void visitDonatePage() const;
    void visitUpdatePage() const;
    void visitBlogPage(const QString &post) const;
    void exit(QWidget *widget) const;

    void checkUpdates(QWidget *parent = nullptr) const;
    bool resetSettings(QWidget *parent = nullptr) const;

    void openOptions(QWidget *parent = nullptr) const;
    void openDeviceManager(QWidget *parent = nullptr) const;
    void openKeyManager(QWidget *parent = nullptr) const;
    void openAndroidExplorer(MainWindow *window) const;
    void openAndroidExplorer(const QString &serial, MainWindow *window) const;
    void takeScreenshot(QWidget *parent) const;
    void takeScreenshot(const QString &serial, QWidget *parent) const;

    QAction *getOpenApk(MainWindow *window) const;
    QAction *getOptimizeApk(MainWindow *window) const;
    QAction *getSignApk(MainWindow *window) const;
    QAction *getInstallApk(MainWindow *window) const;
    QAction *getInstallApk(const QString &serial, MainWindow *window) const;

    QAction *getVisitWebPage(QObject *parent = nullptr) const;
    QAction *getVisitSourcePage(QObject *parent = nullptr) const;
    QAction *getVisitDonatePage(QObject *parent = nullptr) const;
    QAction *getExit(QWidget *widget) const;

    QAction *getCheckUpdates(QWidget *parent) const;
    QAction *getResetSettings(QWidget *parent) const;
    QAction *getOpenOptions(QWidget *parent) const;
    QAction *getOpenDeviceManager(QWidget *parent) const;
    QAction *getOpenKeyManager(QWidget *parent) const;
    QAction *getOpenAndroidExplorer(MainWindow *window) const;
    QAction *getOpenAndroidExplorer(const QString &serial, MainWindow *window) const;
    QAction *getTakeScreenshot(QWidget *parent) const;
    QAction *getTakeScreenshot(const QString &serial, QWidget *parent) const;

    QMenu *getLanguages(QWidget *parent = nullptr) const;

    bool event(QEvent *event) override;

signals:
    void languageChanged();

private:
    ProjectItemsModel &projects;
};

#endif // ACTIONPROVIDER_H
