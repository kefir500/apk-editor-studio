#include "widgets/iconlist.h"
#include "base/application.h"
#include "base/utils.h"
#include <QPainter>
#include <QDragEnterEvent>
#include <QMimeData>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

IconList::IconList(QWidget *parent) : QListView(parent)
{
    setAcceptDrops(true);
}

IconItemsModel *IconList::model() const
{
    return static_cast<IconItemsModel *>(QListView::model());
}

void IconList::setModel(IconItemsModel *model)
{
    QListView::setModel(model);
}

void IconList::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    const bool isImage = (mimeData->hasUrls() && Utils::isImageReadable(mimeData->urls().at(0).path())) || mimeData->hasImage();
    event->setAccepted(isImage);
    setHighlight(true, indexAt(event->pos()));
}

void IconList::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    setHighlight(false);
}

void IconList::dragMoveEvent(QDragMoveEvent *event)
{
    setHighlight(true, indexAt(event->pos()));
}

void IconList::paintEvent(QPaintEvent *event)
{
    QListView::paintEvent(event);
    QPainter painter(viewport());
    painter.fillRect(highlightRect, app->getColor(app->ColorHighlight));
}

void IconList::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
        if (model()) {
            QString iconSource(mimeData->urls().at(0).toLocalFile());
            if (!iconSource.isEmpty()) {
                bool success = false;
                const QModelIndex index = indexAt(event->pos());
                if (index.isValid()) {
                    const QString iconTarget = model()->index(index.row(), IconItemsModel::IconPath).data().toString();
                    success = Utils::copyImage(iconSource, iconTarget);
                } else {
                    for (int row = 0; row < model()->rowCount(); ++row) {
                        const QString iconTarget = model()->index(row, IconItemsModel::IconPath).data().toString();
                        success = Utils::copyImage(iconSource, iconTarget);
                    }
                }
                if (success) {
                    emit model()->dataChanged(index, index);
                }
            }
        }
    } else if (mimeData->hasImage()) {
#ifdef QT_DEBUG
        qDebug() << "TODO: Image dropped";
#endif
    }
    setHighlight(false);
}

void IconList::setHighlight(bool highlight, const QModelIndex &index)
{
    if (!index.isValid()) {
        highlightRect = highlight ? rect() : QRect();
    } else {
        highlightRect = visualRect(index);
    }
    viewport()->update();
}
