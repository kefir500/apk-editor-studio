#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include "editors/fileeditor.h"
#include <QGraphicsView>
#include <QLabel>

// GraphicsView

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent = nullptr);
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void setHighlight(bool highlight);

signals:
    void zoomed(qreal factor);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    qreal zoomFactor;
    const qreal zoomDelta;
};

// ZoomGroup

class ZoomGroup : public QWidget
{
    Q_OBJECT

public:
    explicit ZoomGroup(QWidget *parent = nullptr);
    void setZoomInfo(qreal factor);

signals:
    void zoomIn() const;
    void zoomOut() const;
    void zoomReset() const;

private:
    QLabel *labelZoom;
};

// ImageEditor

class ImageEditor : public FileEditor
{
    Q_OBJECT

public:
    ImageEditor(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() override;
    bool save(const QString &as = QString()) override;

    void setHighlight(bool value);

    static QStringList supportedFormats();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void setImage(const QPixmap &image);
    void setSizeInfo(int width, int height);
    void setSizeInfo(const QSize &size);

    GraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    QLabel *labelSize;
    ZoomGroup *zoomGroup;
};

#endif // IMAGEEDITOR_H
