#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include "editors/baseeditor.h"
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
    void setHighlight(bool value);

signals:
    void zoomed(qreal factor);

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;

private:
    qreal zoomFactor;
    qreal zoomDelta;
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

class ImageEditor : public BaseEditor
{
    Q_OBJECT

public:
    ImageEditor(const QString &filename, QWidget *parent = nullptr);

    bool load() Q_DECL_OVERRIDE;
    bool save(const QString &as = QString()) Q_DECL_OVERRIDE;

    void setHighlight(bool value);

protected:
    QString filter() Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

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
