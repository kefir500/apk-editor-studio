#ifndef MANIFESTVIEW_H
#define MANIFESTVIEW_H

#include "apk/manifestmodel.h"
#include <QTableView>

class ManifestView : public QTableView
{
    Q_OBJECT

public:
    explicit ManifestView(QWidget *parent = 0);
    QSize sizeHint() const Q_DECL_OVERRIDE;

signals:
    void editorRequested(ManifestModel::ManifestRow manifestField);
};

#endif // MANIFESTVIEW_H
