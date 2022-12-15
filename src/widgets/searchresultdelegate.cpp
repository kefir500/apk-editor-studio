#include "widgets/searchresultdelegate.h"
#include "base/searchmodel.h"
#include <QApplication>
#include <QFontDatabase>
#include <QPainter>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

SearchResultDelegate::SearchResultDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
#if defined(Q_OS_WIN)
    font.setFamily("Consolas");
    font.setPointSize(10);
#elif defined(Q_OS_MACOS)
    font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(12);
#elif defined(Q_OS_LINUX)
    font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(10);
#endif
}

void SearchResultDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    if (!SearchModel::isResultIndex(index)) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyleOptionViewItem standardItemOption(option);
    initStyleOption(&standardItemOption, index);
    standardItemOption.text = QString();
    standardItemOption.widget->style()->drawControl(QStyle::CE_ItemViewItem, &standardItemOption, painter, standardItemOption.widget);

    QStyleOptionViewItem itemOption(option);
    initStyleOption(&itemOption, index);
    const auto document = initTextDocument(itemOption, index);
    const int textOffsetX = getTextOffsetX(itemOption);
    const int textOffsetY = (itemOption.rect.height() - document->size().height()) / 2;
    painter->save();
    painter->translate(textOffsetX, itemOption.rect.y() + textOffsetY);
    document->drawContents(painter);
    painter->restore();
}

QString SearchResultDelegate::getText(const QModelIndex &index)
{
    const int lineNumber = index.data(SearchModel::LineNumberRole).toInt();
    const int lineNumberLength = index.data(SearchModel::LineNumberLengthRole).toInt();
    const QString lineContent = index.data().toString();
    return QString("%1  %2").arg(lineNumber, lineNumberLength).arg(lineContent);
}

int SearchResultDelegate::getTextOffsetX(const QStyleOptionViewItem &option)
{
    const auto styleProxy = option.widget->style()->proxy();
    const auto textRect = styleProxy->subElementRect(QStyle::SE_ItemViewItemText, &option, option.widget);
    return textRect.x();
}

std::unique_ptr<QTextDocument> SearchResultDelegate::initTextDocument(const QStyleOptionViewItem &option,
                                                                      const QModelIndex &index) const
{
    auto document = new QTextDocument(getText(index));
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::NoWrap);
    document->setDefaultTextOption(textOption);
    QTextCursor cursor(document);

    QPalette::ColorGroup colorGroup = option.state & QStyle::State_Enabled
        ? QPalette::Normal
        : QPalette::Disabled;
    if (colorGroup == QPalette::Normal && !(option.state & QStyle::State_Active)) {
        colorGroup = QPalette::Inactive;
    }

    // Format the whole line:
    QTextCharFormat lineFormat;
    lineFormat.setFont(font);
    if (option.state & QStyle::State_Selected) {
        lineFormat.setForeground(option.palette.color(colorGroup, QPalette::HighlightedText));
    } else {
        lineFormat.setForeground(option.palette.color(colorGroup, QPalette::Text));
    }
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(lineFormat);

    // Highlight the query result:
    const int lineNumberLength = index.data(SearchModel::LineNumberLengthRole).toInt();
    const int highlightOffset = lineNumberLength + 2; // Offset for the line number
    const int highlightStart = index.data(SearchModel::MatchStartRole).toInt() + highlightOffset;
    const int highlightLength = index.data(SearchModel::MatchLengthRole).toInt();
    cursor.setPosition(highlightStart);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, highlightLength);
    QTextCharFormat resultFormat;
    resultFormat.setForeground(Qt::black);
    resultFormat.setBackground(Qt::yellow);
    resultFormat.setFont(font);
    cursor.setCharFormat(resultFormat);

    return std::unique_ptr<QTextDocument>(document);
}
