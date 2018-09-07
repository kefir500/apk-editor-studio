#include "widgets/resourceviewcontainer.h"
#include "base/application.h"
#include "windows/dialogs.h"
#include <QMenu>

ResourceViewContainer::ResourceViewContainer(QAbstractItemView *view, QWidget *parent) : QWidget(parent)
{
    this->view = view;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(view);
    view->setParent(this);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(view, &QAbstractItemView::activated, [=](const QModelIndex &index) {
        emit requestForEdit(index);
    });
    connect(view, &QWidget::customContextMenuRequested, [=](const QPoint &point) {
        const QModelIndex index = view->indexAt(point);
        if (index.isValid() && !view->model()->hasChildren(index)) {
            openContextMenu(view->viewport()->mapToGlobal(point), index);
        }
    });
}

QAbstractItemModel *ResourceViewContainer::model() const
{
    return view->model();
}

void ResourceViewContainer::openContextMenu(const QPoint &point, const QModelIndex &index)
{
    QMenu menu(this);
    QAction *actionResourceReplace = new QAction(app->loadIcon("replace.png"), tr("&Replace Resource..."), this);
    QAction *actionResourceEdit = new QAction(app->loadIcon("edit.png"), tr("&Edit Resource"), this);
    //: "Resource" is a singular noun in this context.
    QAction *actionResourceOpenDirectory = new QAction(app->loadIcon("explore.png"), tr("&Open Resource Directory"), this);

    connect(actionResourceReplace, &QAction::triggered, [=]() {
        const QString path = getResourcePath(index);
        if (!path.isEmpty()) {
            if (Dialogs::replaceFile(path, this)) {
                emit model()->dataChanged(index, index);
            }
        }
    });

    connect(actionResourceEdit, &QAction::triggered, [=]() {
        emit requestForEdit(index);
    });

    connect(actionResourceOpenDirectory, &QAction::triggered, [=]() {
        const QString path = getResourcePath(index);
        if (!path.isEmpty()) {
            app->explore(path);
        }
    });

    menu.addAction(actionResourceReplace);
    menu.addSeparator();
    menu.addAction(actionResourceEdit);
    menu.addAction(actionResourceOpenDirectory);
    menu.exec(point);
}

QString ResourceViewContainer::getResourcePath(const QModelIndex &index) const
{
    // Check if the underlying model is ResourcesModel:
    auto resourcesModel = qobject_cast<ResourcesModel *>(model());
    if (resourcesModel) {
        return resourcesModel->getResourcePath(index);
    }

    // Check if the underlying model is IconsProxy:
    auto iconsModel = qobject_cast<IconsProxy *>(model());
    if (iconsModel) {
        return iconsModel->getIconPath(index);
    }

    // Check if the underlying model is QFileSystemModel:
    auto filesystemModel = qobject_cast<QFileSystemModel *>(model());
    if (filesystemModel) {
        return filesystemModel->filePath(index);
    }

    return QString();
}
