#include "widgets/resourceabstractview.h"
#include "windows/dialogs.h"
#include "apk/resourcemodelindex.h"
#include <QAbstractItemView>
#include <QBoxLayout>

ResourceAbstractView::ResourceAbstractView(QAbstractItemView *view, QWidget *parent) : QWidget(parent), view(view)
{
    view->setParent(this);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->setMargin(0);

    connect(view, &QAbstractItemView::activated, this, [this](const QModelIndex &index) {
        if (!index.model()->hasChildren(index)) {
            emit editRequested(index);
        }
    });

    connect(view, &QAbstractItemView::customContextMenuRequested, this, [=](const QPoint &point) {
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

    QAction *actionEdit = menu->addAction(QIcon::fromTheme("document-edit"), tr("Edit Resource"));
    connect(actionEdit, &QAction::triggered, this, [=]() {
        emit editRequested(resourceIndex);
    });

    menu->addSeparator();

    QAction *actionReplace = menu->addAction(QIcon::fromTheme("document-swap"), tr("Replace Resource..."));
    connect(actionReplace, &QAction::triggered, this, [&]() {
        resourceIndex.replace(this);
    });

    QAction *actionSaveAs = menu->addAction(QIcon::fromTheme("document-save-as"), tr("Save Resource As..."));
    connect(actionSaveAs, &QAction::triggered, this, [&]() {
        resourceIndex.save(this);
    });

    QAction *actionRemove = menu->addAction(QIcon::fromTheme("document-delete"), tr("Delete Resource"));
    connect(actionRemove, &QAction::triggered, this, [&]() {
        if (!resourceIndex.remove()) {
            QMessageBox::warning(this, QString(), tr("Could not remove the resource."));
        }
    });

    menu->addSeparator();

    //: This string refers to a single resource.
    QAction *actionExplore = menu->addAction(QIcon::fromTheme("folder-open"), tr("Open Resource Directory"));
    connect(actionExplore, &QAction::triggered, [=]() {
        resourceIndex.explore();
    });

    return QSharedPointer<QMenu>(menu);
}
