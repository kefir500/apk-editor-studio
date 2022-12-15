#ifndef SEARCHRESULTDELEGATE_H
#define SEARCHRESULTDELEGATE_H

#include <QStyledItemDelegate>

class QTextDocument;

class SearchResultDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit SearchResultDelegate(QWidget *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    static QString getText(const QModelIndex &index);
    static int getTextOffsetX(const QStyleOptionViewItem &option);
    std::unique_ptr<QTextDocument> initTextDocument(const QStyleOptionViewItem &option,
                                                    const QModelIndex &index) const;

    QFont font;
};

#endif // SEARCHRESULTDELEGATE_H
