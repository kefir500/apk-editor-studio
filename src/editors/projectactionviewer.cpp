#include "editors/projectactionviewer.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"
#include <QEvent>

ProjectActionViewer::ProjectActionViewer(Project *project, QWidget *parent) : ActionViewer(parent)
{
    //: This string refers to a single project (as in "Manager of a project").
    this->title = tr("Project Manager");
    this->icon = app->icons.get("project.png");
    this->project = project;

    btnEditTitle = addButton();
    connect(btnEditTitle, &QPushButton::clicked, [=]() {
        emit titleEditorRequested();
    });

    btnEditIcon = addButton();
    connect(btnEditIcon, &QPushButton::clicked, [=]() {
        const QString iconSource(Dialogs::getOpenImageFilename(this));
        project->setApplicationIcon(iconSource, this);
    });

    btnExplore = addButton();
    connect(btnExplore, &QPushButton::clicked, [=]() {
        Utils::explore(project->getContentsPath());
    });

    btnSave = addButton();
    connect(btnSave, &QPushButton::clicked, [=]() {
        emit apkSaveRequested();
    });

    btnInstall = addButton();
    connect(btnInstall, &QPushButton::clicked, [=]() {
        emit apkInstallRequested();
    });

    connect(project, &Project::stateUpdated, this, &ProjectActionViewer::onProjectUpdated, Qt::QueuedConnection);

    onProjectUpdated();
    retranslate();
}

void ProjectActionViewer::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    ActionViewer::changeEvent(event);
}

void ProjectActionViewer::onProjectUpdated()
{
    setTitle(project->getTitle());
    btnEditTitle->setEnabled(project->getState().canEdit());
    btnEditIcon->setEnabled(project->getState().canEdit());
    btnExplore->setEnabled(project->getState().canExplore());
    btnSave->setEnabled(project->getState().canSave());
    btnInstall->setEnabled(project->getState().canInstall());
}

void ProjectActionViewer::retranslate()
{
    setTitle(project->getTitle());
    tr("Edit APK"); // TODO For future use
    btnEditTitle->setText(tr("Application Title"));
    btnEditIcon->setText(tr("Application Icon"));
    btnExplore->setText(tr("Open Contents"));
    btnSave->setText(tr("Save APK"));
    btnInstall->setText(tr("Install APK"));
}
