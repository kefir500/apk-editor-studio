#include "sheets/projectsheet.h"
#include "windows/dialogs.h"
#include "apk/package.h"
#include "base/utils.h"
#include <QEvent>
#include <QPushButton>

ProjectSheet::ProjectSheet(Package *package, QWidget *parent) : BaseActionSheet(parent)
{
    //: This string refers to a single project (as in "Manager of a project").
    this->title = tr("Project Manager");
    this->icon = QIcon::fromTheme("tool-projectmanager");
    this->package = package;

    btnEditTitle = addButton();
    connect(btnEditTitle, &QPushButton::clicked, this, [this]() {
        emit titleEditorRequested();
    });

    btnEditIcon = addButton();
    connect(btnEditIcon, &QPushButton::clicked, package, [package, this]() {
        const QString iconSource(Dialogs::getOpenImageFilename(this));
        package->setApplicationIcon(iconSource, this);
    });

    btnExplore = addButton();
    connect(btnExplore, &QPushButton::clicked, package, [package]() {
        Utils::explore(package->getContentsPath());
    });

    btnSave = addButton();
    connect(btnSave, &QPushButton::clicked, this, [this]() {
        emit apkSaveRequested();
    });

    btnInstall = addButton();
    connect(btnInstall, &QPushButton::clicked, this, [this]() {
        emit apkInstallRequested();
    });

    connect(package, &Package::stateUpdated, this, &ProjectSheet::onPackageUpdated, Qt::QueuedConnection);

    onPackageUpdated();
    retranslate();
}

void ProjectSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseActionSheet::changeEvent(event);
}

void ProjectSheet::onPackageUpdated()
{
    setTitle(package->getTitle());
    btnEditTitle->setEnabled(package->getState().canEdit());
    btnEditIcon->setEnabled(package->getState().canEdit());
    btnExplore->setEnabled(package->getState().canExplore());
    btnSave->setEnabled(package->getState().canSave());
    btnInstall->setEnabled(package->getState().canInstall());
}

void ProjectSheet::retranslate()
{
    setTitle(package->getTitle());
    tr("Edit APK"); // TODO For future use
    btnEditTitle->setText(tr("Application Title"));
    btnEditIcon->setText(tr("Application Icon"));
    btnExplore->setText(tr("Open Contents"));
    btnSave->setText(tr("Save APK"));
    btnInstall->setText(tr("Install APK"));
}
