#include "windows/yesalwaysdialog.h"
#include "base/application.h"
#include "base/utils.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QStyle>

bool YesAlwaysDialog::ask(const QString &identifier, const QString &question, QWidget *parent)
{
    if (app->settings->getRememberState(identifier)) {
        return true;
    }
    YesAlwaysDialog dialog(identifier, question, parent);
    return dialog.exec();
}

YesAlwaysDialog::YesAlwaysDialog(const QString &identifier, const QString &question, QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto vLayout = new QVBoxLayout(this);
    auto hLayout = new QHBoxLayout;

    const int iconSize = qApp->style()->pixelMetric(QStyle::PM_MessageBoxIconSize, nullptr, this);
    auto icon = new QLabel(this);
    icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    icon->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning, nullptr, this).pixmap(iconSize, iconSize));

    auto label = new QLabel(question, this);
    label->setWordWrap(true);

    auto buttons = new QDialogButtonBox(this);
    //: The "&" is a shortcut key prefix, not an "and" conjunction. Details: https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide#shortcuts
    auto btnAlwaysYes = new QPushButton(tr("Yes, &Always"), this);
    buttons->addButton(QDialogButtonBox::Yes);
    buttons->addButton(btnAlwaysYes, QDialogButtonBox::YesRole);
    buttons->addButton(QDialogButtonBox::No);
    buttons->button(QDialogButtonBox::Yes)->setDefault(true);
    connect(btnAlwaysYes, &QPushButton::clicked, [=]() {
        app->settings->setRememberState(identifier, true);
    });
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    hLayout->addWidget(icon);
    hLayout->addWidget(label);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(buttons);

    adjustSize();
    resize(Utils::scale(400, 0));
}
