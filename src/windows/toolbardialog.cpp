#include "windows/toolbardialog.h"
#include "widgets/poollistwidget.h"
#include "widgets/toolbar.h"
#include "base/application.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

ToolbarDialog::ToolbarDialog(const Toolbar &toolbar, QWidget *parent) : QDialog(parent)
{
    resize(app->scale(500, 400));
    setWindowTitle(tr("Toolbar Customization"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Create layout:

    auto layout = new QGridLayout(this);

    auto usedList = new QListWidget(this);
    auto unusedList = new PoolListWidget(this);

    auto unusedLabel = new QLabel(tr("Current actions:"), this);
    usedList->setIconSize(app->scale(20, 20));
    usedList->setDragDropMode(QAbstractItemView::DragDrop);
    usedList->setDefaultDropAction(Qt::MoveAction);
    connect(usedList, &QListWidget::doubleClicked, this, [=](const QModelIndex &index) {
        QListWidgetItem *item = usedList->takeItem(index.row());
        if (!unusedList->isReusable(item)) {
            unusedList->addItem(item);
        }
    });

    auto usedLabel = new QLabel(tr("Available actions:"), this);
    unusedList->setIconSize(app->scale(20, 20));
    connect(unusedList, &PoolListWidget::pulled, this, [=](QListWidgetItem *item) {
        usedList->addItem(new QListWidgetItem(*item));
    });

    auto dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    auto btnApply = dialogButtons->button(QDialogButtonBox::Apply);
    connect(dialogButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(dialogButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(btnApply, &QPushButton::clicked, this, &QDialog::accepted);

    layout->addWidget(unusedLabel, 0, 0);
    layout->addWidget(usedList, 1, 0);
    layout->addWidget(usedLabel, 0, 1);
    layout->addWidget(unusedList, 1, 1);
    layout->addWidget(dialogButtons, 2, 0, 1, 2);

    // Populate lists:

    QMap<QString, QAction *> unusedActions = toolbar.getAvailableActions();

    for (const auto action : toolbar.getCurrentActions()) {
        const QString identifier = toolbar.getIdentifier(action);
        if (identifier == "separator") {
            usedList->addItem(createSeparatorItem());
        } else if (identifier == "spacer") {
            usedList->addItem(createSpacerItem());
        } else {
            const QAction *action = unusedActions.value(identifier);
            unusedActions.remove(identifier);
            if (action) {
                auto item = new QListWidgetItem(action->icon(), action->text().remove('&'));
                item->setData(Qt::UserRole, identifier);
                item->setData(PoolListWidget::ReusableRole, false);
                usedList->addItem(item);
            }
        }
    }

    QMapIterator<QString, QAction *> it(unusedActions);
    while (it.hasNext()) {
        it.next();
        const QString identifier = it.key();
        const QAction *action = it.value();
        auto item = new QListWidgetItem(action->icon(), action->text().remove('&'));
        item->setData(Qt::UserRole, identifier);
        unusedList->addItem(item, false);
    }
    unusedList->addItem(createSeparatorItem());
    unusedList->addItem(createSpacerItem());

    // Dialog result:

    connect(this, &QDialog::accepted, this, [=]() {
        QStringList actions;
        for (int i = 0; i < usedList->count(); ++i) {
            const QString identifier = usedList->item(i)->data(Qt::UserRole).toString();
            actions.append(identifier);
        }
        emit actionsUpdated(actions);
    });
}

QListWidgetItem *ToolbarDialog::createSeparatorItem() const
{
    QIcon icon = QIcon::fromTheme("toolbar-separator");
    //: Separator is a toolbar element which divides buttons with a vertical line.
    QListWidgetItem *separator = new QListWidgetItem(icon, tr("Separator"));
    separator->setData(Qt::UserRole, "separator");
    separator->setData(PoolListWidget::ReusableRole, true);
    return separator;
}

QListWidgetItem *ToolbarDialog::createSpacerItem() const
{
    QIcon icon = QIcon::fromTheme("toolbar-spacer");
    //: Spacer is a toolbar element which divides buttons with an empty space.
    QListWidgetItem *spacer = new QListWidgetItem(icon, tr("Spacer"));
    spacer->setData(Qt::UserRole, "spacer");
    spacer->setData(PoolListWidget::ReusableRole, true);
    return spacer;
}
