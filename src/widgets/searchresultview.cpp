#include "widgets/searchresultview.h"
#include "widgets/searchresultdelegate.h"

SearchResultView::SearchResultView(QWidget *parent) : QTreeView(parent)
{
    setHeaderHidden(true);
    setItemDelegate(new SearchResultDelegate(this));
    connect(this, &QTreeView::expanded, this, &SearchResultView::fitContent);
    connect(this, &QTreeView::collapsed, this, &SearchResultView::fitContent);
}

void SearchResultView::fitContent()
{
    resizeColumnToContents(0);
}
