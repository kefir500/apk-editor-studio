#ifndef MANIFESTVIEW_H
#define MANIFESTVIEW_H

#include "apk/manifestmodel.h"
#include <QTableView>

class ManifestView : public QTableView
{
    Q_OBJECT

public:
    explicit ManifestView(QWidget *parent = nullptr);

    ManifestModel *model() const;
    void setModel(QAbstractItemModel *model) override;

    QSize sizeHint() const override;

private:
    int selectAndroidApi(const QString &dialogTitle, int defaultApi);

signals:
    void titleEditorRequested(ManifestModel::Row manifestField);
};

#endif // MANIFESTVIEW_H
