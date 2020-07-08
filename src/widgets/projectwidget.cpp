#include "widgets/projectwidget.h"
#include "windows/devicemanager.h"
#include "windows/dialogs.h"
#include "windows/permissioneditor.h"
#include "editors/codeeditor.h"
#include "editors/imageeditor.h"
#include "editors/projectactionviewer.h"
#include "editors/titleeditor.h"
#include "base/application.h"
#include "base/utils.h"
#include "tools/keystore.h"
#include <QDebug>

ProjectWidget::ProjectWidget(Project *project, QWidget *parent) : QTabWidget(parent), project(project)
{
    setMovable(true);
    setTabsClosable(true);

    connect(this, &QTabWidget::currentChanged, [this]() {
        emit currentTabChanged(qobject_cast<Viewer *>(currentWidget()));
    });
    connect(this, &QTabWidget::tabCloseRequested, this, [this](int index) {
        Viewer *tab = static_cast<Viewer *>(widget(index));
        closeTab(tab);
    });

    openProjectTab();
}

void ProjectWidget::openProjectTab()
{
    const QString identifier = "project";
    Viewer *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return;
    }

    ProjectActionViewer *tab = new ProjectActionViewer(project, this);
    tab->setProperty("identifier", identifier);
    connect(tab, &ProjectActionViewer::titleEditorRequested, this, &ProjectWidget::openTitlesTab);
    connect(tab, &ProjectActionViewer::apkSaveRequested, this, &ProjectWidget::saveProject);
    connect(tab, &ProjectActionViewer::apkInstallRequested, this, &ProjectWidget::installProject);
    addTab(tab);
}

void ProjectWidget::openTitlesTab()
{
    const QString identifier = "titles";
    Viewer *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return;
    }

    TitleEditor *editor = new TitleEditor(project, this);
    editor->setProperty("identifier", identifier);
    addTab(editor);
}

void ProjectWidget::openResourceTab(const ResourceModelIndex &index)
{
    const QString path = index.path();
    const QString identifier = path;
    Viewer *existing = getTabByIdentifier(identifier);
    if (existing) {
        setCurrentIndex(indexOf(existing));
        return;
    }

    Editor *editor = nullptr;
    const QString extension = QFileInfo(path).suffix();
    if (CodeEditor::supportedFormats().contains(extension)) {
        editor = new CodeEditor(index, this);
    } else if (ImageEditor::supportedFormats().contains(extension)) {
        editor = new ImageEditor(index, this);
    } else {
        qDebug() << "No suitable editor found for" << extension;
        return;
    }
    editor->setProperty("identifier", identifier);
    addTab(editor);
}

void ProjectWidget::openPermissionEditor()
{
    PermissionEditor permissionEditor(project->manifest, this);
    permissionEditor.exec();
}

bool ProjectWidget::saveTabs()
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

bool ProjectWidget::isUnsaved() const
{
    return project->getState().isModified() || hasUnsavedTabs();
}

bool ProjectWidget::saveProject()
{
    if (hasUnsavedTabs()) {
        const QString question = tr("Do you want to save changes before packing?");
        const int answer = QMessageBox::question(this, QString(), question, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
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
    auto command = new Project::ProjectCommand(project);
    command->add(project->createPackCommand(target), true);
    if (app->settings->getOptimizeApk()) {
        command->add(project->createZipalignCommand(target), false);
    }
    if (app->settings->getSignApk()) {
        auto keystore = Keystore::get(this);
        if (keystore) {
            command->add(project->createSignCommand(keystore.get(), target), false);
        }
    }
    command->run();
    return true;
}

bool ProjectWidget::installProject()
{
    const auto device = Dialogs::getInstallDevice(this);
    if (device.isNull()) {
        return false;
    }

    auto command = new Project::ProjectCommand(project);

    if (isUnsaved()) {
        const QString question = tr("Do you want to save changes and pack the APK before installing?");
        const int answer = QMessageBox::question(this, QString(), question, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch (answer) {
        case QMessageBox::Yes:
        case QMessageBox::Save: {
            saveTabs();
            const QString target = Dialogs::getSaveApkFilename(project, this);
            if (target.isEmpty()) {
                delete command;
                return false;
            }
            command->add(project->createPackCommand(target), true);
            if (app->settings->getOptimizeApk()) {
                command->add(project->createZipalignCommand(), false);
            }
            if (app->settings->getSignApk()) {
                auto keystore = Keystore::get(this);
                if (keystore) {
                    command->add(project->createSignCommand(keystore.get()), false);
                }
            }
            break;
        }
        case QMessageBox::No:
        case QMessageBox::Discard:
            break;
        default:
            return false;
        }
    }

    command->add(project->createInstallCommand(device.getSerial()));
    command->run();
    return true;
}

bool ProjectWidget::exploreProject()
{
    return Utils::explore(project->getContentsPath());
}

bool ProjectWidget::closeProject()
{
    if (project->getState().isModified()) {
        const QString question = tr("Are you sure you want to close this APK?\nAny unsaved changes will be lost.");
        const int answer = QMessageBox::question(this, QString(), question);
        if (answer != QMessageBox::Yes) {
            return false;
        }
    }
    return app->projects.close(project);
}

int ProjectWidget::addTab(Viewer *tab)
{
    // TODO Tab title is not retranslated on language change
    const int tabIndex = QTabWidget::addTab(tab, tab->getIcon(), tab->getTitle());
    setCurrentIndex(tabIndex);
    auto editor = qobject_cast<Editor *>(tab);
    if (editor) {
        connect(editor, &Editor::saved, this, [this]() {
            // Project save indicator:
            const_cast<ProjectState &>(project->getState()).setModified(true);
        });
        connect(editor, &Editor::modifiedStateChanged, this, [=](bool modified) {
            // Tab save indicator:
            const QString indicator = QString("%1 ").arg(QChar(0x2022));
            const int tabIndex = indexOf(editor);
            QString tabTitle = tabText(tabIndex);
            const bool titleHasModifiedMark = tabTitle.startsWith(indicator);
            if (modified && !titleHasModifiedMark) {
                tabTitle.prepend(indicator);
                setTabText(tabIndex, tabTitle);
            } else if (!modified && titleHasModifiedMark) {
                tabTitle.remove(0, indicator.length());
                setTabText(tabIndex, tabTitle);
            }
        });
    }
    connect(tab, &Viewer::titleChanged, this, [=](const QString &title) {
        setTabText(indexOf(tab), title);
    });
    connect(tab, &Viewer::iconChanged, this, [=](const QIcon &icon) {
        setTabIcon(indexOf(tab), icon);
    });
    return tabIndex;
}

bool ProjectWidget::closeTab(Viewer *tab)
{
    if (!tab->finalize()) {
        return false;
    }
    delete tab;
    return true;
}

bool ProjectWidget::hasUnsavedTabs() const
{
    for (int i = 0; i < count(); ++i) {
        auto editor = qobject_cast<Editor *>(widget(i));
        if (editor && editor->isModified()) {
            return true;
        }
    }
    return false;
}

Viewer *ProjectWidget::getTabByIdentifier(const QString &identifier) const
{
    for (int index = 0; index < count(); ++index) {
        Viewer *tab = static_cast<Viewer *>(widget(index));
        if (tab->property("identifier") == identifier) {
            return tab;
        }
    }
    return nullptr;
}
