#ifndef ACTIONPROVIDER_H
#define ACTIONPROVIDER_H

#include <QObject>

class QAction;
class QMenu;

class ActionProvider : public QObject
{
    Q_OBJECT

public:
    ActionProvider(QObject *parent = nullptr) : QObject(parent) {}

    void visitWebPage() const;
    void visitSourcePage() const;
    void visitDonatePage() const;
    void visitUpdatePage() const;
    void visitBlogPage(const QString &post) const;

    void checkUpdates(QWidget *parent = nullptr) const;
    bool resetSettings(QWidget *parent = nullptr) const;

    void openOptions(QWidget *parent = nullptr) const;
    void openDeviceManager(QWidget *parent = nullptr) const;
    void openKeyManager(QWidget *parent = nullptr) const;
    void openAndroidExplorer(QWidget *parent = nullptr) const;
    void takeScreenshot(QWidget *parent) const;
    void takeScreenshot(const QString &serial, QWidget *parent) const;

    QAction *getOpenApk(QWidget *parent) const;
    QAction *getOptimizeApk(QWidget *parent) const;
    QAction *getSignApk(QWidget *parent) const;
    QAction *getInstallApk(QWidget *parent) const;

    QAction *getFind(QWidget *parent) const;
    QAction *getFindNext(QWidget *parent) const;
    QAction *getFindPrevious(QWidget *parent) const;
    QAction *getReplace(QWidget *parent) const;

    QAction *getZoomIn(QWidget *parent) const;
    QAction *getZoomOut(QWidget *parent) const;
    QAction *getZoomReset(QWidget *parent) const;

    QAction *getVisitWebPage(QObject *parent = nullptr) const;
    QAction *getVisitSourcePage(QObject *parent = nullptr) const;
    QAction *getVisitDonatePage(QObject *parent = nullptr) const;
    QAction *getExit(QWidget *widget) const;

    QAction *getCheckUpdates(QWidget *parent) const;
    QAction *getResetSettings(QWidget *parent) const;
    QAction *getOpenOptions(QWidget *parent) const;
    QAction *getOpenDeviceManager(QWidget *parent) const;
    QAction *getOpenKeyManager(QWidget *parent) const;
    QAction *getOpenAndroidExplorer(QWidget *parent) const;
    QAction *getTakeScreenshot(QWidget *parent) const;
    QAction *getTakeScreenshot(const QString &serial, QWidget *parent) const;

    QMenu *getLanguages(QWidget *parent = nullptr) const;

    bool event(QEvent *event) override;

signals:
    void languageChanged();
};

#endif // ACTIONPROVIDER_H
