#include "widgets/resourcesview.h"
#include "windows/dialogs.h"
#include "base/application.h"

ResourcesView::ResourcesView(QAbstractItemView *view, QWidget *parent) : QWidget(parent), view(view)
{
    view->setParent(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->setMargin(0);

    connect(view, &QAbstractItemView::activated, [this](const QModelIndex &index) {
        emit editRequested(index);
    });

    connect(view, &QAbstractItemView::customContextMenuRequested, [=](const QPoint &point) {
        const QModelIndex index = view->indexAt(point);
        if (index.isValid() && !view->model()->hasChildren(index)) {
            auto menu = generateContextMenu(index, view);
            if (menu) {
                menu->exec(view->viewport()->mapToGlobal(point));
            }
        }
    });
}

void ResourcesView::setModel(QAbstractItemModel *model)
{
    view->setModel(model);
}

QSharedPointer<ResourceMenu> ResourcesView::generateContextMenu(const ResourceModelIndex &resourceIndex, QWidget *parent)
{
    const QString resourcePath = resourceIndex.path();
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
