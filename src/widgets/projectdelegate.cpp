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
    QStyledItemDelegate::paint(painter, option, index);

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
