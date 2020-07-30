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

    void openApk(MainWindow *window = nullptr);
    void openApk(const QString &path, MainWindow *window = nullptr);
    void openApk(const QStringList &paths, MainWindow *window = nullptr);
    void optimizeApk(MainWindow *window = nullptr);
    void optimizeApk(const QStringList &paths, MainWindow *window = nullptr);
    void signApk(MainWindow *window = nullptr);
    void signApk(const QStringList &paths, MainWindow *window = nullptr);
    void signApk(const QStringList &paths, const Keystore *keystore, MainWindow *window = nullptr);
    void installApk(MainWindow *window = nullptr);
    void installApk(const QString &serial, MainWindow *window = nullptr);
    void installApk(const QStringList &paths, const QString &serial, MainWindow *window = nullptr);
    bool closeApk(Project *project);

    void visitWebPage();
    void visitSourcePage();
    void visitDonatePage();
    void visitUpdatePage();
    void visitBlogPage(const QString &post);
    void exit(QWidget *widget);

    void addToRecent(const Project *project) const;
    void checkUpdates(QWidget *parent = nullptr) const;
    bool resetSettings(QWidget *parent = nullptr) const;

    void openOptions(QWidget *parent = nullptr);
    void openDeviceManager(QWidget *parent = nullptr);
    void openKeyManager(QWidget *parent = nullptr);
    void openAndroidExplorer(MainWindow *window);
    void openAndroidExplorer(const QString &serial, MainWindow *window);
    void takeScreenshot(QWidget *parent);
    void takeScreenshot(const QString &serial, QWidget *parent);

    QAction *getOpenApk(MainWindow *window);
    QAction *getOptimizeApk(MainWindow *window);
    QAction *getSignApk(MainWindow *window);
    QAction *getInstallApk(MainWindow *window);
    QAction *getInstallApk(const QString &serial, MainWindow *window);

    QAction *getVisitWebPage(QObject *parent = nullptr);
    QAction *getVisitSourcePage(QObject *parent = nullptr);
    QAction *getVisitDonatePage(QObject *parent = nullptr);
    QAction *getExit(QWidget *widget);

    QAction *getCheckUpdates(QWidget *parent);
    QAction *getResetSettings(QWidget *parent);
    QAction *getOpenOptions(QWidget *parent);
    QAction *getOpenDeviceManager(QWidget *parent);
    QAction *getOpenKeyManager(QWidget *parent);
    QAction *getOpenAndroidExplorer(MainWindow *window);
    QAction *getOpenAndroidExplorer(const QString &serial, MainWindow *window);
    QAction *getTakeScreenshot(QWidget *parent);
    QAction *getTakeScreenshot(const QString &serial, QWidget *parent);

    QMenu *getLanguages(QWidget *parent = nullptr);
    QMenu *getRecent(MainWindow *window);

    bool event(QEvent *event) override;

signals:
    void languageChanged();

private:
    QAction *getClearRecent(QObject *parent = nullptr);
    QAction *getNoRecent(QObject *parent = nullptr);

    ProjectItemsModel &projects;
};

#endif // ACTIONPROVIDER_H
