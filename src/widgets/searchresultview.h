#ifndef SEARCHRESULTVIEW_H
#define SEARCHRESULTVIEW_H

#include <QTreeView>

class SearchResultView : public QTreeView
{
    Q_OBJECT

public:
    explicit SearchResultView(QWidget *parent = nullptr);

private:
    void fitContent();
};

#endif // SEARCHRESULTVIEW_H
