#include "sheets/projectsheet.h"
#include "windows/dialogs.h"
#include "base/utils.h"
#include <QEvent>

ProjectSheet::ProjectSheet(Project *project, QWidget *parent) : BaseActionSheet(parent)
{
    //: This string refers to a single project (as in "Manager of a project").
    this->title = tr("Project Manager");
    this->icon = QIcon::fromTheme("tool-projectmanager");
    this->project = project;

    btnEditTitle = addButton();
    connect(btnEditTitle, &QPushButton::clicked, this, [this]() {
        emit titleEditorRequested();
    });

    btnEditIcon = addButton();
    connect(btnEditIcon, &QPushButton::clicked, project, [project, this]() {
        const QString iconSource(Dialogs::getOpenImageFilename(this));
        project->setApplicationIcon(iconSource, this);
    });

    btnExplore = addButton();
    connect(btnExplore, &QPushButton::clicked, project, [project]() {
        Utils::explore(project->getContentsPath());
    });

    btnSave = addButton();
    connect(btnSave, &QPushButton::clicked, this, [this]() {
        emit apkSaveRequested();
    });

    btnInstall = addButton();
    connect(btnInstall, &QPushButton::clicked, this, [this]() {
        emit apkInstallRequested();
    });

    connect(project, &Project::stateUpdated, this, &ProjectSheet::onProjectUpdated, Qt::QueuedConnection);

    onProjectUpdated();
    retranslate();
}

void ProjectSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseActionSheet::changeEvent(event);
}

void ProjectSheet::onProjectUpdated()
{
    setTitle(project->getTitle());
    btnEditTitle->setEnabled(project->getState().canEdit());
    btnEditIcon->setEnabled(project->getState().canEdit());
    btnExplore->setEnabled(project->getState().canExplore());
    btnSave->setEnabled(project->getState().canSave());
    btnInstall->setEnabled(project->getState().canInstall());
}

void ProjectSheet::retranslate()
{
    setTitle(project->getTitle());
    tr("Edit APK"); // TODO For future use
    btnEditTitle->setText(tr("Application Title"));
    btnEditIcon->setText(tr("Application Icon"));
    btnExplore->setText(tr("Open Contents"));
    btnSave->setText(tr("Save APK"));
    btnInstall->setText(tr("Install APK"));
}
