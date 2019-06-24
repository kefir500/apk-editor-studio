#include "widgets/resourceabstractview.h"
#include "windows/dialogs.h"
#include "base/application.h"
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
        ResourceModelIndex index = view->indexAt(point);
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

QSharedPointer<QMenu> ResourceAbstractView::generateContextMenu(ResourceModelIndex &resourceIndex)
{
    const QString resourcePath = resourceIndex.path();
    if (resourcePath.isEmpty()) {
        return QSharedPointer<QMenu>(nullptr);
    }

    auto menu = new QMenu(this);

    QAction *actionEdit = menu->addAction(app->icons.get("edit.png"), tr("Edit Resource"));
    connect(actionEdit, &QAction::triggered, [=]() {
        emit editRequested(resourceIndex);
    });

    menu->addSeparator();

    QAction *actionReplace = menu->addAction(app->icons.get("replace.png"), tr("Replace Resource..."));
    connect(actionReplace, &QAction::triggered, [&]() {
        resourceIndex.replace();
    });

    QAction *actionSaveAs = menu->addAction(app->icons.get("save-as.png"), tr("Save Resource As..."));
    connect(actionSaveAs, &QAction::triggered, [=]() {
        resourceIndex.save();
    });

    QAction *actionRemove = menu->addAction(app->icons.get("remove.png"), tr("Delete Resource"));
    connect(actionRemove, &QAction::triggered, [&]() {
        if (!resourceIndex.remove()) {
            QMessageBox::warning(this, QString(), tr("Could not remove the resource."));
        }
    });

    menu->addSeparator();

    //: This string refers to a single resource.
    QAction *actionExplore = menu->addAction(app->icons.get("explore.png"), tr("Open Resource Directory"));
    connect(actionExplore, &QAction::triggered, [=]() {
        resourceIndex.explore();
    });

    return QSharedPointer<QMenu>(menu);
}
