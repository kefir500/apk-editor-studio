#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

class BaseSheet;
class Package;
class ResourceModelIndex;
class QTabWidget;

class Project : public QObject
{
    Q_OBJECT

public:
    Project(Package *package, QWidget *parent = nullptr);

    void openProjectTab();
    void openTitlesTab();
    void openResourceTab(const ResourceModelIndex &index);
    void openPermissionEditor();
    void openPackageRenamer();
    void openSignatureViewer();

    bool saveTabs();
    bool saveProject();
    bool installProject();
    bool exploreProject();

    Package *getPackage() const;
    BaseSheet *getCurrentTab() const;
    QTabWidget *tabs() const;
    bool isUnsaved() const;

signals:
    void currentTabChanged(BaseSheet *tab);

private:
    int addTab(BaseSheet *tab);
    bool closeTab(BaseSheet *tab);
    void setCurrentTab(BaseSheet *tab);

    bool hasUnsavedTabs() const;
    BaseSheet *getTabByIdentifier(const QString &identifier) const;
    QWidget *parentWidget() const;

    Package *package;
    QTabWidget *tabWidget;
};

#endif // PROJECT_H
