#include "widgets/manifestview.h"
#include "widgets/itembuttondelegate.h"
#include "base/application.h"
#include <QEvent>
#include <QHeaderView>

ManifestView::ManifestView(QWidget *parent) : QTableView(parent)
{
    setMouseTracking(true);
    viewport()->setAttribute(Qt::WA_Hover, true);
    horizontalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    ItemButtonDelegate *buttonDelegate = new ItemButtonDelegate(this);
    setItemDelegate(buttonDelegate);
    connect(buttonDelegate, &ItemButtonDelegate::clicked, [=](int row) {
        emit editorRequested(static_cast<ManifestModel::ManifestRow>(row));
    });
}

QSize ManifestView::sizeHint() const
{
    return QSize(app->scale(240), 117);
}
