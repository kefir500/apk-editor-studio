#include "windows/permissioneditor.h"
#include "base/application.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QDesktopServices>

PermissionEditor::PermissionEditor(Manifest *manifest, QWidget *parent) : QDialog(parent)
{
    //: This string refers to multiple permissions (as in "Editor of permissions").
    setWindowTitle(tr("Permission Editor"));
    setWindowIcon(app->icons.get("permissions.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(app->scale(550, 400));

    auto viewport = new QWidget(this);
    viewport->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto scroll = new QScrollArea(this);
    scroll->setWidget(viewport);
    scroll->setWidgetResizable(true);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(scroll);

    auto grid = new QGridLayout(viewport);
    auto permissions = manifest->getPermissionList();
    for (int i = 0; i < permissions.count(); ++i) {
        auto permission = permissions.at(i);
        auto permissionName = permission->getName();
        auto labelTitle = new QLabel(permissionName, this);
        grid->addWidget(labelTitle, i, 0);

        auto btnHelp = new QToolButton(this);
        btnHelp->setToolTip(tr("Documentation"));
        btnHelp->setIcon(app->icons.get("help.png"));
        connect(btnHelp, &QToolButton::clicked, [=]() {
            const QString url("https://developer.android.com/reference/android/Manifest.permission.html#%1");
            QDesktopServices::openUrl(url.arg(permissionName.mid(QString("android.permission.").length())));
        });
        grid->addWidget(btnHelp, i, 1);

        auto btnRemove = new QToolButton(this);
        btnRemove->setToolTip(tr("Remove"));
        btnRemove->setIcon(app->icons.get("remove.png"));
        connect(btnRemove, &QToolButton::clicked, [=]() {
            delete labelTitle;
            delete btnHelp;
            delete btnRemove;
            forRemoval.append(permission);
        });
        grid->addWidget(btnRemove, i, 2);

        if (!permissionName.startsWith("android.permission.")) {
            btnHelp->hide();
        }
    }

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(this, &QDialog::accepted, [=]() {
        for (auto *permission : forRemoval) {
            manifest->removePermission(permission);
        }
    });
    layout->addWidget(buttons);
}
