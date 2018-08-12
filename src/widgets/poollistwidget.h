#ifndef POOLLISTWIDGET_H
#define POOLLISTWIDGET_H

#include <QListWidget>

class PoolListWidget : public QListWidget {

public:
    using QListWidget::addItem;

    enum PoolItemRole {
        IdentifierRole = Qt::UserRole,
        ReusableRole
    };

    PoolListWidget(QWidget *parent = 0);
    void addItem(QListWidgetItem *item, bool reusable);
    void startDrag(Qt::DropActions supportedActions) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
};

#endif // POOLLISTWIDGET_H
