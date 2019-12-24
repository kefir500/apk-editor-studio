#include "widgets/projectlistitemdelegate.h"
#include "apk/projectitemsmodel.h"
#include "base/application.h"
#include <QPainter>

// TODO Add indicator for unsaved projects

void ProjectListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption = option;
    initStyleOption(&itemOption, index);
    const bool isSelected = (itemOption.state & QStyle::State_Selected);

#ifdef Q_OS_WIN
    QPalette palette = itemOption.palette;
    palette.setColor(QPalette::All, QPalette::HighlightedText, palette.color(QPalette::Active, QPalette::Text));
    palette.setColor(QPalette::All, QPalette::Highlight, palette.base().color().darker(108));
    itemOption.palette = palette;
#endif

    // Prepare state icon:

    const QModelIndex stateIndex = index.sibling(index.row(), ProjectItemsModel::StateColumn);
    const QIcon stateIcon = stateIndex.data(Qt::DecorationRole).value<QIcon>();
    const int stateIconSideMaximum = option.rect.height() - 2;
    const QSize stateIconSize = stateIcon.actualSize(QSize(stateIconSideMaximum, stateIconSideMaximum));

    // Prepare text:

    QRect textRect = itemOption.rect;
    textRect.setLeft(itemOption.decorationSize.width() + 8);
    textRect.setRight(itemOption.rect.width() - stateIconSize.width() - 8 * 2);
    const QString text = painter->fontMetrics().elidedText(itemOption.text, Qt::ElideMiddle, textRect.width());
    itemOption.text.clear();

    // Draw base control:

    const QStyle *style = option.widget ? option.widget->style() : app->style();
    style->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter);

    // Draw text:

    const QPalette::ColorRole textRole = isSelected ? QPalette::HighlightedText : QPalette::Text;
    style->drawItemText(painter, textRect, Qt::AlignVCenter, itemOption.palette, true, text, textRole);

    // Draw state icon:

    const QRect stateIconRect(option.rect.width() - stateIconSize.width() - 8,
                              option.rect.center().y() - stateIconSize.height() / 2,
                              stateIconSize.width(),
                              stateIconSize.height());
    const QIcon::Mode stateIconMode = isSelected ? QIcon::Selected : QIcon::Normal;
    stateIcon.paint(painter, stateIconRect, Qt::AlignRight, stateIconMode);
}
