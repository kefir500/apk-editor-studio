#ifndef POOLLISTWIDGET_H
#define POOLLISTWIDGET_H

#include <QListWidget>

class PoolListWidget : public QListWidget
{
    Q_OBJECT

public:
    enum Role {
        ReusableRole = Qt::UserRole + 1
    };

    using QListWidget::addItem;

    PoolListWidget(QWidget *parent = nullptr);

    void addItem(QListWidgetItem *item, bool reusable);
    void startDrag(Qt::DropActions supportedActions) override;
    void dropEvent(QDropEvent *event) override;

    static bool isReusable(QListWidgetItem *item);

signals:
    void pulled(QListWidgetItem *item);
};

#endif // POOLLISTWIDGET_H
