#include "widgets/iconlist.h"
#include "widgets/decorationsizedelegate.h"
#include "base/application.h"
#include "base/utils.h"
#include <QPainter>
#include <QDragEnterEvent>
#include <QMimeData>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

IconList::IconList(QWidget *parent) : QTreeView(parent)
{
    setAcceptDrops(true);
    setItemDelegate(new DecorationSizeDelegate(QSize(36, 36), this));
    setHeaderHidden(true);
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

void IconList::setModel(QAbstractItemModel *model)
{
    if (this->model()) {
        disconnect(this->model(), &IconItemsModel::ready, this, &IconList::expandApplicationIcons);
    }

    if (model) {
        auto iconModel = qobject_cast<IconItemsModel *>(model);
        Q_ASSERT(iconModel);
        for (int column = 1; column < model->columnCount(); ++column) {
            hideColumn(column);
        }
        QTreeView::setModel(iconModel);
        connect(iconModel, &IconItemsModel::ready, this, &IconList::expandApplicationIcons);
        expandApplicationIcons();
    }
}

IconItemsModel *IconList::model() const
{
    return static_cast<IconItemsModel *>(QTreeView::model());
}

void IconList::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    const bool isImage = (mimeData->hasUrls() && Utils::isImageReadable(mimeData->urls().at(0).path())) || mimeData->hasImage();
    event->setAccepted(isImage);
    rubberBand->setVisible(isImage);
}

void IconList::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    rubberBand->hide();
}

void IconList::dragMoveEvent(QDragMoveEvent *event)
{
    if (!model()->hasChildren(indexAt(event->pos()))) {
        rubberBand->setGeometry(visualRect(indexAt(event->pos())));
    } else {
        rubberBand->setGeometry(QRect());
    }
}

void IconList::dropEvent(QDropEvent *event)
{
    rubberBand->hide();
    if (!model()) {
        return;
    }
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
        QString path(mimeData->urls().at(0).toLocalFile());
        if (!path.isEmpty()) {
            const QModelIndex index = indexAt(event->pos());
            if (index.isValid() && rect().contains(event->pos())) {
                model()->replaceResource(index, path);
            }
        }
    } else if (mimeData->hasImage()) {
#ifdef QT_DEBUG
        qDebug() << "TODO: Image dropped";
#endif
    }
}

void IconList::expandApplicationIcons()
{
    expand(model()->index(IconItemsModel::ApplicationRow, 0));
}
