#ifndef IMAGESHEET_H
#define IMAGESHEET_H

#include "sheets/basefilesheet.h"
#include <QGraphicsView>

class QLabel;
class QRubberBand;

// GraphicsView

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsView(QWidget *parent = nullptr);
    void zoomIn();
    void zoomOut();
    void zoomReset();

signals:
    void zoomed(qreal factor);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    qreal zoomFactor;
    const qreal zoomDelta = 1.25;
};

// ImageSheet

class ImageSheet : public BaseFileSheet
{
    Q_OBJECT

public:
    ImageSheet(const ResourceModelIndex &index, QWidget *parent = nullptr);

    bool load() override;
    bool save(const QString &as = QString()) override;
    bool saveAs() override;

protected:
    void changeEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setImage(const QPixmap &image);
    void retranslate();

    GraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    QLabel *sizeLabel;
    QLabel *sizeValueLabel;
    QLabel *zoomLabel;
    QLabel *zoomValueLabel;
    QRubberBand *rubberBand;
};

#endif // IMAGESHEET_H
