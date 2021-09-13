#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QListView>

class LogDelegate;
class LogModel;

class LogView : public QListView
{
    Q_OBJECT

public:
    explicit LogView(QWidget *parent = nullptr);

    LogModel *model() const;
    void setModel(QAbstractItemModel *model) override;
    QSize sizeHint() const override;

private:
    LogDelegate *delegate;
};

#endif // LOGVIEW_H
