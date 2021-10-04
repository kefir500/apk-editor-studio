#include "sheets/imagesheet.h"
#include "base/application.h"
#include "base/fileformatlist.h"
#include "base/utils.h"
#include "windows/dialogs.h"
#include <QAction>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QMimeData>
#include <QRubberBand>
#include <QToolButton>
#include <QWheelEvent>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

// ImageSheet

ImageSheet::ImageSheet(const ResourceModelIndex &index, QWidget *parent) : BaseFileSheet(index, parent)
{
    setSheetTitle(index.path().section('/', -2));
    setAcceptDrops(true);

    scene = new QGraphicsScene(this);
    view = new GraphicsView(this);
    view->setScene(scene);
    rubberBand = new QRubberBand(QRubberBand::Rectangle, view);

    auto actionZoomIn = app->actions.getZoomIn(this);
    auto actionZoomOut = app->actions.getZoomOut(this);
    auto actionZoomReset = app->actions.getZoomReset(this);

    connect(actionZoomIn, &QAction::triggered, view, &GraphicsView::zoomIn);
    connect(actionZoomOut, &QAction::triggered, view, &GraphicsView::zoomOut);
    connect(actionZoomReset, &QAction::triggered, view, &GraphicsView::zoomReset);

    addActionSeparator();
    addAction(actionZoomIn);
    addAction(actionZoomOut);
    addAction(actionZoomReset);

    sizeLabel = new QLabel(this);
    sizeValueLabel = new QLabel(this);

    zoomLabel = new QLabel(this);
    auto zoomValueLabel = new QLabel(this);
    auto btnZoomIn = new QToolButton(this);
    auto btnZoomOut = new QToolButton(this);
    auto btnZoomReset = new QToolButton(this);
    btnZoomIn->setDefaultAction(actionZoomIn);
    btnZoomOut->setDefaultAction(actionZoomOut);
    btnZoomReset->setDefaultAction(actionZoomReset);
    auto zoomControlsLayout = new QHBoxLayout;
    zoomControlsLayout->setMargin(0);
    zoomControlsLayout->addWidget(btnZoomIn);
    zoomControlsLayout->addWidget(btnZoomReset);
    zoomControlsLayout->addWidget(btnZoomOut);
    zoomControlsLayout->addWidget(zoomValueLabel);
    zoomControlsLayout->addStretch();

    auto controls = new QWidget(this);
    auto controlsLayout = new QFormLayout(controls);
    controlsLayout->addRow(sizeLabel, sizeValueLabel);
    controlsLayout->addRow(zoomLabel, zoomControlsLayout);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->addWidget(controls);
    layout->setMargin(0);

    connect(view, &GraphicsView::zoomed, this, [zoomValueLabel](qreal factor) {
        const int percentage = static_cast<int>(factor * 100);
        zoomValueLabel->setText(QString("%1%").arg(percentage));
    });

    retranslate();
    load();
}

bool ImageSheet::load()
{
    QPixmap pixmap(index.path());
    if (pixmap.isNull()) {
        return false;
    }

    setImage(pixmap);
    setSheetIcon(pixmap);
    setModified(false);

    return true;
}

bool ImageSheet::save(const QString &as)
{
    if (as.isEmpty()) {
        if (!pixmapItem->pixmap().save(index.path())) {
            return false;
        }
        setModified(false);
        emit saved();
        return true;
    } else {
        return pixmapItem->pixmap().save(as);
    }
}

bool ImageSheet::saveAs()
{
    const QString filename = index.path();
    const QString destination = Dialogs::getSaveImageFilename(filename, this);
    if (destination.isEmpty()) {
        return false;
    }
    return save(destination);
}

void ImageSheet::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }
    BaseFileSheet::changeEvent(event);
}

void ImageSheet::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    const bool isImage = (mimeData->hasUrls() && Utils::isImageReadable(mimeData->urls().at(0).path())) || mimeData->hasImage();
    event->setAccepted(isImage);
    rubberBand->setVisible(isImage);
}

void ImageSheet::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    rubberBand->hide();
}

void ImageSheet::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
        const QString file = mimeData->urls().at(0).toLocalFile();
        QPixmap pixmap(file);
        if (!pixmap.isNull()) {
            setImage(pixmap);
            setModified(true);
        }
    } else if (mimeData->hasImage()) {
#ifdef QT_DEBUG
        qDebug() << "TODO: Image dropped";
#endif
    }
    rubberBand->hide();
}

void ImageSheet::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    rubberBand->setGeometry(geometry());
}

void ImageSheet::setImage(const QPixmap &image)
{
    scene->clear();
    view->zoomReset();
    view->setSceneRect(0, 0, image.width(), image.height());
    pixmapItem = scene->addPixmap(image);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    sizeValueLabel->setText(QString("%1x%2").arg(image.width()).arg(image.height()));
}

void ImageSheet::retranslate()
{
    sizeLabel->setText(tr("Size"));
    zoomLabel->setText(tr("Zoom"));
}

// GraphicsView

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setAcceptDrops(false);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    zoomReset();
}

void GraphicsView::zoomIn()
{
    if (zoomFactor < 50) {
        const qreal scale = zoomDelta;
        this->scale(scale, scale);
        emit zoomed(zoomFactor *= scale);
    }
}

void GraphicsView::zoomOut()
{
    if (zoomFactor > 0.05) {
        const qreal scale = 1 / qreal(zoomDelta);
        this->scale(scale, scale);
        emit zoomed(zoomFactor *= scale);
    }
}

void GraphicsView::zoomReset()
{
    zoomFactor = 1.0;
    resetTransform();
    emit zoomed(1);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    const qreal delta = event->angleDelta().y();
    delta > 0 ? zoomIn() : zoomOut();
}

void GraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->resetTransform();
    painter->fillRect(mapFromScene(rect).boundingRect(), Qt::Dense7Pattern);
}
