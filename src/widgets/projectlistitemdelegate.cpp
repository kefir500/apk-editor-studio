#include "widgets/projectlistitemdelegate.h"
#include "apk/projectsmodel.h"
#include "base/application.h"
#include <QPainter>

// TODO Add indicator for unsaved projects

ProjectListItemDelegate::ProjectListItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    iconReady = app->loadPixmap("state-ready.png");
    iconUnpacking = app->loadPixmap("state-open.png");
    iconPacking = app->loadPixmap("state-save.png");
    iconInstalling = app->loadPixmap("state-install.png");
    iconError = app->loadPixmap("state-error.png");
}

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

    const int projectState = index.data(ProjectsModel::ProjectStateRole).toInt();
    const bool projectError = index.data(ProjectsModel::ProjectErrorRole).toBool();
    QIcon stateIcon;
    if (projectError) {
        stateIcon = iconError;
    } else {
        switch (projectState) {
        case Project::ProjectReady:
            stateIcon = iconReady;
            break;
        case Project::ProjectUnpacking:
            stateIcon = iconUnpacking;
            break;
        case Project::ProjectPacking:
        case Project::ProjectSigning:
        case Project::ProjectOptimizing:
            stateIcon = iconPacking;
            break;
        case Project::ProjectInstalling:
            stateIcon = iconInstalling;
            break;
        }
    }
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
    qDebug() << style;
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
