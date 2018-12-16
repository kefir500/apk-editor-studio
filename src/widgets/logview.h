#ifndef LOGVIEW_H
#define LOGVIEW_H

#include "apk/logmodel.h"
#include <QListView>
#include <QVariantAnimation>

class LogView : public QListView
{
    Q_OBJECT

public:
    explicit LogView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVariantAnimation *loading;
};

#endif // LOGVIEW_H
