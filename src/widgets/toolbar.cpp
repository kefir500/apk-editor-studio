#include "widgets/toolbar.h"
#include "widgets/spacer.h"
#include "base/application.h"

Toolbar::Toolbar(QWidget *parent) : QToolBar(parent)
{
    setMovable(false);
    setIconSize(app->scale(30, 30));
}

void Toolbar::reinitialize()
{
    clear();
    QStringList entries = app->settings->getToolbar();
    foreach (const QString &entry, entries) {
        if (entry == "separator") {
            addSeparator();
        } else if (entry == "spacer") {
            addWidget(new Spacer(this));
        } else {
            addAction(app->toolbar.value(entry));
        }
    }
}
