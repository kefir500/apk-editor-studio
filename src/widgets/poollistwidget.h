#ifndef POOLLISTWIDGET_H
#define POOLLISTWIDGET_H

#include <QListWidget>

class PoolListWidget : public QListWidget
{
public:
    using QListWidget::addItem;

    enum Role {
        IdentifierRole = Qt::UserRole + 1,
        ReusableRole
    };

    PoolListWidget(QWidget *parent = nullptr);
    void addItem(QListWidgetItem *item, bool reusable);
    void startDrag(Qt::DropActions supportedActions) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // POOLLISTWIDGET_H
