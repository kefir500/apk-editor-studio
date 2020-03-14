#ifndef DESELECTABLELISTVIEW_H
#define DESELECTABLELISTVIEW_H

#include <QListView>

class DeselectableListView : public QListView
{
public:
    DeselectableListView(QWidget *parent) : QListView(parent) {}

private:
    void mousePressEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
};

#endif // DESELECTABLELISTVIEW_H
