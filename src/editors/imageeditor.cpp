#include "editors/imageeditor.h"
#include "base/application.h"
#include "base/fileformatlist.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsColorizeEffect>
#include <QWheelEvent>
#include <QMimeData>
#include <QFormLayout>
#include <QToolButton>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

// ImageEditor

ImageEditor::ImageEditor(const QString &filename, QWidget *parent) : BaseEditor(filename, QPixmap(), parent)
{
    title = filename.section('/', -2);

    setAcceptDrops(true);

    scene = new QGraphicsScene(this);
    view = new GraphicsView(this);
    view->setScene(scene);

    zoomGroup = new ZoomGroup(this);
    labelSize = new QLabel(this);

    QWidget *controls = new QWidget(this);
    QFormLayout *controlsLayout = new QFormLayout(controls);
    controlsLayout->addRow(tr("Size"), labelSize);
    controlsLayout->addRow(tr("Zoom"), zoomGroup);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->addWidget(controls);

    connect(zoomGroup, &ZoomGroup::zoomIn, view, &GraphicsView::zoomIn);
    connect(zoomGroup, &ZoomGroup::zoomOut, view, &GraphicsView::zoomOut);
    connect(zoomGroup, &ZoomGroup::zoomReset, view, &GraphicsView::zoomReset);
    connect(view, &GraphicsView::zoomed, zoomGroup, &ZoomGroup::setZoomInfo);

    load();
}

bool ImageEditor::load()
{
    QPixmap pixmap(filename);
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

void ImageEditor::setImage(const QPixmap &image)
{
    scene->clear();
    view->zoomReset();
    view->setSceneRect(0, 0, image.width(), image.height());
    pixmapItem = scene->addPixmap(image);
    pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    setSizeInfo(image.size());
}

bool ImageEditor::save(const QString &as)
{
    if (as.isEmpty()) {
        const bool success = pixmapItem->pixmap().save(filename);
        setModified(!success);
        return success;
    } else {
        const bool success = pixmapItem->pixmap().save(as);
        return success;
    }
}

void ImageEditor::setHighlight(bool value)
{
    view->setHighlight(value);
}

QStringList ImageEditor::supportedFormats()
{
    return FileFormatList::forReadableImages().getExtensions();
}

void ImageEditor::setSizeInfo(int width, int height)
{
    labelSize->setText(QString("%1x%2").arg(width).arg(height));
}

void ImageEditor::setSizeInfo(const QSize &size)
{
    setSizeInfo(size.width(), size.height());
}

void ImageEditor::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls() || mimeData->hasImage()) {
        setHighlight(true);
        event->acceptProposedAction();
    }
}

void ImageEditor::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    setHighlight(false);
}

void ImageEditor::dropEvent(QDropEvent *event)
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
    setHighlight(false);
}

// ZoomGroup

ZoomGroup::ZoomGroup(QWidget *parent) : QWidget(parent)
{
    QToolButton *btnZoomIn = new QToolButton(this);
    QToolButton *btnZoomOut = new QToolButton(this);
    QToolButton *btnZoomNormal = new QToolButton(this);
    btnZoomIn->setIcon(app->loadIcon("zoom-in.png"));
    btnZoomOut->setIcon(app->loadIcon("zoom-out.png"));
    btnZoomNormal->setIcon(app->loadIcon("zoom.png"));

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
    const int percentage = factor * 100;
    labelZoom->setText(QString("%1%").arg(percentage));
}

// GraphicsView

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent), zoomDelta(1.25)
{
    setAcceptDrops(false);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHighlight(false);
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

void GraphicsView::setHighlight(bool value)
{
    QColor background = app->getColor(app->ColorHighlight);
    setStyleSheet(value ? QString("GraphicsView {background: %1}").arg(background.name(QColor::HexArgb)) : QString());
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
