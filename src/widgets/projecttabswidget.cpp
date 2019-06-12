#include "widgets/projecttabswidget.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "editors/codeeditor.h"
#include "editors/imageeditor.h"
#include "base/application.h"
#include "base/utils.h"
#include <QDebug>

ProjectTabsWidget::ProjectTabsWidget(Project *project, QWidget *parent) : QTabWidget(parent), project(project)
{
    setMovable(true);
    setTabsClosable(true);

    connect(this, &ProjectTabsWidget::tabCloseRequested, [=](int index) {
        Viewer *tab = static_cast<Viewer *>(widget(index));
        closeTab(tab);
    });

    openProjectTab();
}

ProjectActionViewer *ProjectTabsWidget::openProjectTab()
{
    const QString identifier = "project";
    Viewer *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return static_cast<ProjectActionViewer *>(existing);
    }

    ProjectActionViewer *tab = new ProjectActionViewer(project, this);
    tab->setProperty("identifier", identifier);
    connect(tab, &ProjectActionViewer::titleEditorRequested, this, &ProjectTabsWidget::openTitlesTab);
    connect(tab, &ProjectActionViewer::apkSaveRequested, this, &ProjectTabsWidget::saveProject);
    connect(tab, &ProjectActionViewer::apkInstallRequested, this, &ProjectTabsWidget::installProject);
    addTab(tab);
    return tab;
}

TitleEditor *ProjectTabsWidget::openTitlesTab()
{
    const QString identifier = "titles";
    Viewer *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return static_cast<TitleEditor *>(existing);
    }

    TitleEditor *editor = new TitleEditor(project, this);
    editor->setProperty("identifier", identifier);
    addTab(editor);
    return editor;
}

Viewer *ProjectTabsWidget::openResourceTab(const ResourceModelIndex &index)
{
    const QString path = index.path();
    const QString identifier = path;
    Viewer *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return existing;
    }

    Editor *editor = nullptr;
    const QString extension = QFileInfo(path).suffix();
    if (CodeEditor::supportedFormats().contains(extension)) {
        editor = new CodeEditor(index, this);
    } else if (ImageEditor::supportedFormats().contains(extension)) {
        editor = new ImageEditor(index, this);
    } else {
        qDebug() << "No suitable editor found for" << extension;
        return nullptr;
    }
    editor->setProperty("identifier", identifier);
    addTab(editor);
    return editor;
}

bool ProjectTabsWidget::saveTabs()
{
    bool result = true;
    for (int index = 0; index < count(); ++index) {
        Editor *tab = qobject_cast<Editor *>(widget(index));
        if (tab && !tab->save()) {
            result = false;
        }
    }
    return result;
}

bool ProjectTabsWidget::isUnsaved() const
{
    return project->getState().isModified() || hasUnsavedTabs();
}

bool ProjectTabsWidget::saveProject()
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
    project->save(target);
    return true;
}

bool ProjectTabsWidget::installProject()
{
    DeviceManager devices(this);
    const Device *device = devices.getTargetDevice();
    if (!device) {
        return false;
    }

    if (isUnsaved()) {
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

bool ProjectTabsWidget::exploreProject()
{
    return Utils::explore(project->getContentsPath());
}

bool ProjectTabsWidget::closeProject()
{
    if (project->getState().isModified()) {
        const int answer = QMessageBox::question(this, QString(), tr("Are you sure you want to close this APK?\nAny unsaved changes will be lost."));
        if (answer != QMessageBox::Yes) {
            return false;
        }
    }
    return app->projects.close(project);
}

int ProjectTabsWidget::addTab(Viewer *tab)
{
    const int tabIndex = QTabWidget::addTab(tab, tab->getIcon(), tab->getTitle());
    setCurrentIndex(tabIndex);
    auto editor = qobject_cast<Editor *>(tab);
    if (editor) {
        connect(editor, &Editor::saved, [=]() {
            // Project save indicator:
            const_cast<ProjectState &>(project->getState()).setModified(true);
        });
        connect(editor, &Editor::savedStateChanged, [=](bool tabSaved) {
            // Tab save indicator:
            const QString indicator = QString("%1 ").arg(QChar(0x2022));
            const int tabIndex = indexOf(editor);
            QString tabTitle = tabText(tabIndex);
            const bool titleModified = tabTitle.startsWith(indicator);
            if (!tabSaved && !titleModified) {
                tabTitle.prepend(indicator);
                setTabText(tabIndex, tabTitle);
            } else if (tabSaved && titleModified) {
                tabTitle.remove(0, indicator.length());
                setTabText(tabIndex, tabTitle);
            }
        });
    }
    connect(tab, &Viewer::titleChanged, [=](const QString &title) {
        setTabText(indexOf(tab), title);
    });
    connect(tab, &Viewer::iconChanged, [=](const QIcon &icon) {
        setTabIcon(indexOf(tab), icon);
    });
    return tabIndex;
}

bool ProjectTabsWidget::closeTab(Viewer *editor)
{
    if (!editor->finalize()) {
        return false;
    }
    delete editor;
    return true;
}

bool ProjectTabsWidget::hasUnsavedTabs() const
{
    for (int i = 0; i < count(); ++i) {
        auto editor = qobject_cast<Editor *>(widget(i));
        if (editor && editor->isModified()) {
            return true;
        }
    }
    return false;
}

Viewer *ProjectTabsWidget::getTabByIdentifier(const QString &identifier) const
{
    for (int index = 0; index < count(); ++index) {
        Viewer *tab = static_cast<Viewer *>(widget(index));
        if (tab->property("identifier") == identifier) {
            return tab;
        }
    }
    return nullptr;
}
