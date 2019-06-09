#include "widgets/resourceabstractview.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include "base/utils.h"
#include <QMenu>

ResourceAbstractView::ResourceAbstractView(QAbstractItemView *view, QWidget *parent) : QWidget(parent), view(view)
{
    view->setParent(this);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->setMargin(0);

    connect(view, &QAbstractItemView::activated, this, &ResourceAbstractView::editRequested);

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

void ResourceAbstractView::setModel(QAbstractItemModel *model)
{
    view->setModel(model);
}

QSharedPointer<QMenu> ResourceAbstractView::generateContextMenu(const ResourceModelIndex &resourceIndex)
{
    const QString resourcePath = resourceIndex.path();
    if (resourcePath.isEmpty()) {
        return QSharedPointer<QMenu>(nullptr);
    }

    auto menu = new QMenu(this);

    QAction *actionEdit = menu->addAction(app->icons.get("edit.png"), tr("&Edit Resource"));
    connect(actionEdit, &QAction::triggered, [=]() {
        emit editRequested(resourceIndex);
    });

    menu->addSeparator();

    QAction *actionReplace = menu->addAction(app->icons.get("replace.png"), tr("Re&place Resource..."));
    connect(actionReplace, &QAction::triggered, [=]() {
        if (Dialogs::replaceFile(resourcePath, this)) {
            auto model = const_cast<QAbstractItemModel *>(resourceIndex.model());
            emit model->dataChanged(resourceIndex, resourceIndex);
        }
    });

    QAction *actionSaveAs = menu->addAction(app->icons.get("save-as.png"), tr("Save Resource &As..."));
    connect(actionSaveAs, &QAction::triggered, [=]() {
        Dialogs::copyFile(resourcePath, this);
    });

    QAction *actionRemove = menu->addAction(app->icons.get("remove.png"), tr("&Remove Resource"));
    connect(actionRemove, &QAction::triggered, [=]() {
        view->model()->removeRow(resourceIndex.row(), resourceIndex.parent());
    });

    menu->addSeparator();

    //: This string refers to a single resource.
    QAction *actionExplore = menu->addAction(app->icons.get("explore.png"), tr("&Open Resource Directory"));
    connect(actionExplore, &QAction::triggered, [=]() {
        Utils::explore(resourcePath);
    });

    return QSharedPointer<QMenu>(menu);
}
