#include "widgets/resourcesbaseview.h"
#include "windows/dialogs.h"
#include "base/application.h"

QSharedPointer<ResourceMenu> ResourcesBaseView::generateContextMenu(const QPersistentModelIndex &resourceIndex, const QString &resourcePath, QWidget *parent)
{
    if (resourcePath.isEmpty()) {
        return QSharedPointer<ResourceMenu>(nullptr);
    }

    auto menu = new ResourceMenu(parent);
    menu->getSaveAction()->setVisible(false);

    menu->connect(menu, &ResourceMenu::editClicked, [=]() {
        emit editRequested(resourceIndex);
    });

    menu->connect(menu, &ResourceMenu::replaceClicked, [=]() {
        if (Dialogs::replaceFile(resourcePath, parent)) {
            auto model = const_cast<QAbstractItemModel *>(resourceIndex.model());
            emit model->dataChanged(resourceIndex, resourceIndex);
        }
    });

    menu->connect(menu, &ResourceMenu::saveAsClicked, [=]() {
        Dialogs::copyFile(resourcePath, parent);
    });

    menu->connect(menu, &ResourceMenu::exploreClicked, [=]() {
        app->explore(resourcePath);
    });

    return QSharedPointer<ResourceMenu>(menu);
}
