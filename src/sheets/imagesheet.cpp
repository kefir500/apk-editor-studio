#include "sheets/imagesheet.h"
#include "base/fileformatlist.h"
#include "base/utils.h"
#include "windows/dialogs.h"
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
    title = index.path().section('/', -2);

    setAcceptDrops(true);

    scene = new QGraphicsScene(this);
    view = new GraphicsView(this);
    view->setScene(scene);
    rubberBand = new QRubberBand(QRubberBand::Rectangle, view);

    zoomGroup = new ZoomGroup(this);
    sizeValueLabel = new QLabel(this);

    auto controls = new QWidget(this);
    auto controlsLayout = new QFormLayout(controls);
    sizeLabel = new QLabel(this);
    zoomLabel = new QLabel(this);
    controlsLayout->addRow(sizeLabel, sizeValueLabel);
    controlsLayout->addRow(zoomLabel, zoomGroup);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->addWidget(controls);
    layout->setMargin(0);

    connect(zoomGroup, &ZoomGroup::zoomIn, view, &GraphicsView::zoomIn);
    connect(zoomGroup, &ZoomGroup::zoomOut, view, &GraphicsView::zoomOut);
    connect(zoomGroup, &ZoomGroup::zoomReset, view, &GraphicsView::zoomReset);
    connect(view, &GraphicsView::zoomed, zoomGroup, &ZoomGroup::setZoomInfo);

    retranslate();
    load();
}

bool ImageSheet::load()
{
    QPixmap pixmap(index.path());
    if (pixmap.isNull()) {
        return false;
    }

    // Set image:
    setImage(pixmap);
    setModified(false);

    // Set tab icon:
    QIcon icon;
    icon.addPixmap(pixmap);
    this->icon = icon;
    emit iconChanged(icon);

    return true;
}

void ImageSheet::setImage(const QPixmap &image)
{
    scene->clear();
    view->zoomReset();
    view->setSceneRect(0, 0, image.width(), image.height());
    pixmapItem = scene->addPixmap(image);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    setSizeInfo(image.size());
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

void ImageSheet::setSizeInfo(int width, int height)
{
    sizeValueLabel->setText(QString("%1x%2").arg(width).arg(height));
}

void ImageSheet::setSizeInfo(const QSize &size)
{
    setSizeInfo(size.width(), size.height());
}

void ImageSheet::retranslate()
{
    sizeLabel->setText(tr("Size"));
    zoomLabel->setText(tr("Zoom"));
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

// ZoomGroup

ZoomGroup::ZoomGroup(QWidget *parent) : QWidget(parent)
{
    QToolButton *btnZoomIn = new QToolButton(this);
    QToolButton *btnZoomOut = new QToolButton(this);
    QToolButton *btnZoomNormal = new QToolButton(this);
    btnZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
    btnZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
    btnZoomNormal->setIcon(QIcon::fromTheme("zoom-normal"));

    labelZoom = new QLabel(this);

    QHBoxLayout *layoutZoom = new QHBoxLayout(this);
    layoutZoom->setMargin(0);
    layoutZoom->addWidget(btnZoomIn);
    layoutZoom->addWidget(btnZoomNormal);
    layoutZoom->addWidget(btnZoomOut);
    layoutZoom->addWidget(labelZoom);
    layoutZoom->addStretch();

    connect(btnZoomIn, &QToolButton::clicked, this, &ZoomGroup::zoomIn);
    connect(btnZoomOut, &QToolButton::clicked, this, &ZoomGroup::zoomOut);
    connect(btnZoomNormal, &QToolButton::clicked, this, &ZoomGroup::zoomReset);
}

void ZoomGroup::setZoomInfo(qreal factor)
{
    const int percentage = static_cast<int>(factor * 100);
    labelZoom->setText(QString("%1%").arg(percentage));
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
