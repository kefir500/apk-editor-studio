#include "widgets/resourcesview.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include <QMenu>

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
            auto menu = generateContextMenu(index);
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

QSharedPointer<QMenu> ResourcesView::generateContextMenu(const ResourceModelIndex &resourceIndex)
{
    const QString resourcePath = resourceIndex.path();
    if (resourcePath.isEmpty()) {
        return QSharedPointer<QMenu>(nullptr);
    }

    auto menu = new QMenu(this);

    QAction *actionEdit = menu->addAction(app->loadIcon("edit.png"), tr("&Edit Resource"));
    connect(actionEdit, &QAction::triggered, [=]() {
        emit editRequested(resourceIndex);
    });

    menu->addSeparator();

    QAction *actionReplace = menu->addAction(app->loadIcon("replace.png"), tr("&Replace Resource..."));
    connect(actionReplace, &QAction::triggered, [=]() {
        if (Dialogs::replaceFile(resourcePath, this)) {
            auto model = const_cast<QAbstractItemModel *>(resourceIndex.model());
            emit model->dataChanged(resourceIndex, resourceIndex);
        }
    });

    QAction *actionSaveAs = menu->addAction(app->loadIcon("save-as.png"), tr("Save Resource &As..."));
    connect(actionSaveAs, &QAction::triggered, [=]() {
        Dialogs::copyFile(resourcePath, this);
    });

    menu->addSeparator();

    //: This string refers to a single resource.
    QAction *actionExplore = menu->addAction(app->loadIcon("explore.png"), tr("&Open Resource Directory"));
    connect(actionExplore, &QAction::triggered, [=]() {
        app->explore(resourcePath);
    });

    return QSharedPointer<QMenu>(menu);
}
