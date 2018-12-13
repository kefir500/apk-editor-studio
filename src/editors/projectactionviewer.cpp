#include "editors/projectactionviewer.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"
#include <QEvent>

ProjectActionViewer::ProjectActionViewer(Project *project, QWidget *parent) : ActionViewer(parent)
{
    //: This string refers to a single project (as in "Manager of a project").
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
                Utils::copyImage(iconSource, iconTarget);
            }
        }
    });

    btnExplore = addButton();
    connect(btnExplore, &QPushButton::clicked, [=]() {
        app->explore(project->getContentsPath());
    });

    btnSave = addButton();
    connect(btnSave, &QPushButton::clicked, [=]() {
        emit apkSaveRequested();
    });

    btnInstall = addButton();
    connect(btnInstall, &QPushButton::clicked, [=]() {
        emit apkInstallRequested();
    });

    connect(project, &Project::changed, this, &ProjectActionViewer::onProjectUpdated, Qt::QueuedConnection);

    onProjectUpdated();
    retranslate();
}

void ProjectActionViewer::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        QWidget::changeEvent(event);
    }
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
