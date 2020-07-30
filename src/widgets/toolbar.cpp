#include "widgets/toolbar.h"
#include "widgets/spacer.h"
#include "windows/toolbardialog.h"
#include "base/utils.h"
#include <QEvent>

Toolbar::Toolbar(QWidget *parent) : QToolBar(parent)
{
    setMovable(false);

    const QList<int> availableSizes = {16, 30, 40}; // Hardcoded icon sizes
    const int closestSize = Utils::roundToNearest(Utils::scale(30, 30).width(), availableSizes);
    setIconSize(QSize(closestSize, closestSize));

    actionCustomize = new QAction(QIcon::fromTheme("toolbar-customize"), {}, this);
    connect(actionCustomize, &QAction::triggered, this, [this]() {
        ToolbarDialog toolbarDialog(*this, this);
        connect(&toolbarDialog, &ToolbarDialog::actionsUpdated, this, &Toolbar::initialize);
        connect(&toolbarDialog, &ToolbarDialog::actionsUpdated, this, &Toolbar::updated);
        toolbarDialog.exec();
    });

    initialize({});
}

QString Toolbar::getIdentifier(const QAction *action)
{
    return action->property("identifier").toString();
}

const QList<QAction *> &Toolbar::getCurrentActions() const
{
    return currentActions;
}

const QMap<QString, QAction *> &Toolbar::getAvailableActions() const
{
    return availableActions;
}

void Toolbar::addActionToPool(const QString &identifier, QAction *action)
{
    availableActions.insert(identifier, action);
    setIdentifier(action, identifier);
}

void Toolbar::initialize(const QStringList &actions)
{
    // Remove actions:

    for (auto *action : this->actions()) {
        const auto identifier = getIdentifier(action);
        if (identifier == "separator" || identifier == "spacer") {
            delete action;
        }
    }
    clear();
    currentActions.clear();

    // Add actions:

    for (const QString &identifier : actions) {
        if (identifier == "separator") {
            currentActions.append(addSeparator());
        } else if (identifier == "spacer") {
            currentActions.append(addSpacer());
        } else {
            auto action = availableActions.value(identifier);
            addAction(action);
            currentActions.append(action);
        }
    }
    if (!actions.contains("spacer")) {
        addSpacer();
    }
    addSeparator();
    addAction(actionCustomize);
}

void Toolbar::setIdentifier(QAction *action, const QString &identifier)
{
    action->setProperty("identifier", identifier);
}

QAction *Toolbar::addSeparator()
{
    auto separator = QToolBar::addSeparator();
    setIdentifier(separator, "separator");
    return separator;
}

QAction *Toolbar::addSpacer()
{
    auto spacer = addWidget(new Spacer(this));
    setIdentifier(spacer, "spacer");
    return spacer;
}

void Toolbar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        actionCustomize->setText(tr("Customize Toolbar..."));
    }
    QToolBar::changeEvent(event);
}
