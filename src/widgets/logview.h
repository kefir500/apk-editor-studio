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
    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QVariantAnimation *loading;
};

#endif // LOGVIEW_H
