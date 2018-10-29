#include "widgets/projectwidget.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "editors/codeeditor.h"
#include "editors/imageeditor.h"
#include "apk/resourcesmodel.h"
#include "base/application.h"
#include <QModelIndex>
#include <QFileInfo>

ProjectWidget::ProjectWidget(Project *project, QWidget *parent) : QTabWidget(parent)
{
    this->project = project;

    setMovable(true);
    setTabsClosable(true);
    connect(this, &ProjectWidget::tabCloseRequested, [=](int index) {
        BaseEditor *tab = static_cast<BaseEditor *>(widget(index));
        closeTab(tab);
    });

    openProjectTab();
}

BaseEditor *ProjectWidget::currentTab() const
{
    return static_cast<BaseEditor *>(currentWidget());
}

bool ProjectWidget::saveProject()
{
    if (hasUnsavedTabs()) {
        const int answer = QMessageBox::question(this, QString(), tr("Do you want to save changes before packing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch (answer) {
        case QMessageBox::Yes:
        case QMessageBox::Save:
            saveTabs();
            break;
        case QMessageBox::No:
        case QMessageBox::Discard:
            break;
        default:
            return false;
        }
    }
    const QString target = Dialogs::getSaveApkFilename(project, this);
    if (target.isEmpty()) {
        return false;
    }
    project->pack(target);
    return true;
}

bool ProjectWidget::installProject()
{
    DeviceManager devices(this);
    const Device *device = devices.getDevice();
    if (!device) {
        return false;
    }

    if (hasUnsavedProject()) {
        const QString question = tr("Do you want to save changes and pack the APK before installing?");
        const int answer = QMessageBox::question(this, QString(), question, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch (answer) {
        case QMessageBox::Yes:
        case QMessageBox::Save: {
            saveTabs();
            const QString target = Dialogs::getSaveApkFilename(project, this);
            if (target.isEmpty()) {
                return false;
            }
            project->saveAndInstall(target, device->getSerial());
            return true;
        }
        case QMessageBox::No:
        case QMessageBox::Discard:
            break;
        default:
            return false;
        }
    }

    project->install(device->getSerial());
    return true;
}

bool ProjectWidget::exploreProject()
{
    app->explore(project->getContentsPath());
    return true;
}

bool ProjectWidget::closeProject()
{
    if (project->getModifiedState()) {
        const int answer = QMessageBox::question(this, QString(), tr("Are you sure you want to close this APK?\nAny unsaved changes will be lost."));
        if (answer != QMessageBox::Yes) {
            return false;
        }
    }
    return app->projects.close(project);
}

ProjectManager *ProjectWidget::openProjectTab()
{
    const QString identifier = "project";
    BaseEditor *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return static_cast<ProjectManager *>(existing);
    }

    ProjectManager *tab = new ProjectManager(project, this);
    tab->setProperty("identifier", identifier);
    connect(tab, &ProjectManager::titleEditorRequested, this, &ProjectWidget::openTitlesTab);
    connect(tab, &ProjectManager::apkSaveRequested, this, &ProjectWidget::saveProject);
    connect(tab, &ProjectManager::apkInstallRequested, this, &ProjectWidget::installProject);
    addTab(tab);
    return tab;
}

TitleEditor *ProjectWidget::openTitlesTab()
{
    const QString identifier = "titles";
    BaseEditor *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return static_cast<TitleEditor *>(existing);
    }

    TitleEditor *editor = new TitleEditor(project, this);
    editor->setProperty("identifier", identifier);
    addTab(editor);
    return editor;
}

BaseEditor *ProjectWidget::openResourceTab(const QString &path, const QPixmap &icon)
{
    const QString identifier = path;
    BaseEditor *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return existing;
    }

    BaseEditor *editor = nullptr;
    const QString fileSuffix = QFileInfo(path).suffix();
    if (app->formats.isCode(fileSuffix)) {
        editor = new CodeEditor(path, icon, this);
    } else if (app->formats.isImage(fileSuffix)) {
        editor = new ImageEditor(path, this);
    } else {
        qDebug() << "No suitable editor found for" << fileSuffix;
        return nullptr;
    }
    editor->setProperty("identifier", identifier);
    addTab(editor);

    return editor;
}

bool ProjectWidget::saveTabs()
{
    bool result = true;
    for (int index = 0; index < count(); ++index) {
        BaseEditor *tab = static_cast<BaseEditor *>(widget(index));
        if (!tab->save()) {
            result = false;
        }
    }
    return result;
}

bool ProjectWidget::closeTab(BaseEditor *editor)
{
    if (editor && editor->commit()) {
        delete editor;
        return true;
    }
    return false;
}

bool ProjectWidget::hasUnsavedProject() const
{
    return project->getModifiedState() || hasUnsavedTabs();
}

bool ProjectWidget::hasUnsavedTabs() const
{
    for (int i = 0; i < count(); ++i) {
        BaseEditor *tab = static_cast<BaseEditor *>(widget(i));
        if (tab->isModified()) {
            return true;
        }
    }
    return false;
}

Project *ProjectWidget::getProject() const
{
    return project;
}

int ProjectWidget::addTab(BaseEditor *tab)
{
    const int tabIndex = QTabWidget::addTab(tab, tab->getIcon(), tab->getTitle());
    setCurrentIndex(tabIndex);
    connect(tab, &BaseEditor::savedStateChanged, [=](bool tabSaved) {
        // Project save indicator:
        if (!tabSaved) {
            project->setModified(true);
        }
        // Tab save indicator:
        const QString indicator = QString(" %1").arg(QChar(0x2022));
        const int tabIndex = indexOf(tab);
        QString tabTitle = tabText(tabIndex);
        const bool titleModified = tabTitle.endsWith(indicator);
        if (!tabSaved && !titleModified) {
            tabTitle.append(indicator);
            setTabText(tabIndex, tabTitle);
        } else if (tabSaved && titleModified) {
            tabTitle.chop(indicator.length());
            setTabText(tabIndex, tabTitle);
        }
    });
    connect(tab, &BaseEditor::titleChanged, [=](const QString &title) {
        setTabText(indexOf(tab), title);
    });
    connect(tab, &BaseEditor::iconChanged, [=](const QIcon &icon) {
        setTabIcon(indexOf(tab), icon);
    });
    return tabIndex;
}

BaseEditor *ProjectWidget::getTabByIdentifier(const QString &identifier)
{
    for (int index = 0; index < count(); ++index) {
        BaseEditor *tab = static_cast<BaseEditor *>(widget(index));
        if (tab->property("identifier") == identifier) {
            return tab;
        }
    }
    return nullptr;
}
