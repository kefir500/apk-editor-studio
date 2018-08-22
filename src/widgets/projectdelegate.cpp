#include "widgets/projectdelegate.h"
#include "apk/projectsmodel.h"
#include "base/application.h"
#include <QPainter>

// TODO Add indicator for unsaved projects

ProjectDelegate::ProjectDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    iconReady = app->loadPixmap("state-ready.png");
    iconUnpacking = app->loadPixmap("state-open.png");
    iconPacking = app->loadPixmap("state-save.png");
    iconInstalling = app->loadPixmap("state-install.png");
    iconError = app->loadPixmap("state-error.png");
}

void ProjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Draw base control:

    QStyleOptionViewItem itemOption = option;
    initStyleOption(&itemOption, index);
    QRect textRect = itemOption.rect;
    textRect.setLeft(option.decorationSize.width() + 8);
    textRect.setWidth(itemOption.rect.width() - iconReady.width() - 8 * 2);
    const QString text = painter->fontMetrics().elidedText(itemOption.text, Qt::ElideMiddle, textRect.width());
    itemOption.text.clear();
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter);
    painter->drawText(textRect, text, QTextOption(Qt::AlignVCenter));

    // Draw state icon:

    const int projectState = index.data(ProjectsModel::ProjectStateRole).toInt();
    const bool projectError = index.data(ProjectsModel::ProjectErrorRole).toBool();
    QPixmap stateIcon;
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

    const qreal x = option.rect.width() - stateIcon.width() - 8;
    const qreal y = option.rect.center().y() - stateIcon.height() / 2;
    painter->drawPixmap(x, y, stateIcon);
}
