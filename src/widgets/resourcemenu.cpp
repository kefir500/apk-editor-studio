#include "widgets/resourcemenu.h"
#include "windows/dialogs.h"
#include "base/application.h"

ResourceMenu::ResourceMenu(QWidget *parent) : QMenu(parent)
{
    actionEdit = addAction(app->loadIcon("edit.png"), QString());
    separatorEdit = addSeparator();
    actionReplace = addAction(app->loadIcon("replace.png"), QString());
    separatorReplace = addSeparator();
    actionSave = addAction(app->loadIcon("save.png"), QString());
    actionSaveAs = addAction(app->loadIcon("save-as.png"), QString());
    separatorSave = addSeparator();
    //: "Resource" is a singular noun in this context.
    actionExplore = addAction(app->loadIcon("explore.png"), QString());

    connect(actionEdit, &QAction::triggered, [=]() {
        emit editClicked();
    });
    connect(actionReplace, &QAction::triggered, [=]() {
        emit replaceClicked();
    });
    connect(actionSave, &QAction::triggered, [=]() {
        emit saveClicked();
    });
    connect(actionSaveAs, &QAction::triggered, [=]() {
        emit saveAsClicked();
    });
    connect(actionExplore, &QAction::triggered, [=]() {
        emit exploreClicked();
    });

    retranslate();
}

QAction *ResourceMenu::getEditAction()
{
    return actionEdit;
}

QAction *ResourceMenu::getReplaceAction()
{
    return actionReplace;
}

QAction *ResourceMenu::getSaveAction()
{
    return actionSave;
}

QAction *ResourceMenu::getSaveAsAction()
{
    return actionSaveAs;
}

QAction *ResourceMenu::getExploreAction()
{
    return actionExplore;
}

void ResourceMenu::setEnabled(bool enabled)
{
    actionEdit->setEnabled(enabled);
    actionReplace->setEnabled(enabled);
    actionSave->setEnabled(enabled);
    actionSaveAs->setEnabled(enabled);
    actionExplore->setEnabled(enabled);
}

void ResourceMenu::retranslate()
{
    setTitle(tr("&Resource"));
    actionEdit->setText(tr("&Edit Resource"));
    actionReplace->setText(tr("&Replace Resource..."));
    actionSave->setText(tr("&Save Resource"));
    actionSaveAs->setText(tr("Save Resource &As..."));
    actionExplore->setText(tr("&Open Resource Directory"));
}

void ResourceMenu::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        QWidget::changeEvent(event);
    }
}
