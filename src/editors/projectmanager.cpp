#include "editors/projectmanager.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include <QEvent>

ProjectManager::ProjectManager(Project *project, QWidget *parent) : QuickTab(parent)
{
    this->title = tr("Project Manager");
    this->icon = app->loadIcon("project.png");
    this->project = project;

    btnEditTitle = addButton();
    connect(btnEditTitle, &QPushButton::clicked, [=]() {
        emit titleEditorRequested();
    });

    btnEditIcon = addButton();
    connect(btnEditIcon, &QPushButton::clicked, [=]() {
        const QString iconSource(Dialogs::getOpenImageFilename(this));
        if (!iconSource.isEmpty()) {
            for (int row = 0; row < project->iconsProxy.rowCount(); ++row) {
                const QString iconTarget = project->iconsProxy.index(row, IconsProxy::IconPath).data().toString();
                app->replaceImage(iconTarget, iconSource);
            }
        }
    });
    btnSave = addButton();
    connect(btnSave, &QPushButton::clicked, [=]() {
        emit apkSaveRequested();
    });
    btnInstall = addButton();
    connect(btnInstall, &QPushButton::clicked, [=]() {
        emit apkInstallRequested();
    });

    setEnabled(project->getState() != Project::ProjectNone);
    connect(project, &Project::unpacked, this, &ProjectManager::setEnabled, Qt::QueuedConnection);
    connect(project, &Project::dataChanged, this, [=]() {
        setTitle(project->getTitle());
    }, Qt::QueuedConnection);

    retranslate();
}

void ProjectManager::setSaveButtonEnabled(bool enabled)
{
    btnSave->setEnabled(enabled);
}

void ProjectManager::setInstallButtonEnabled(bool enabled)
{
    btnInstall->setEnabled(enabled);
}

void ProjectManager::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        QWidget::changeEvent(event);
    }
}

void ProjectManager::retranslate()
{
    setTitle(project->getTitle());
    tr("Edit APK"); // TODO For future use
    btnEditTitle->setText(tr("Application Title"));
    btnEditIcon->setText(tr("Application Icon"));
    btnSave->setText(tr("Save APK"));
    btnInstall->setText(tr("Install APK"));
}
