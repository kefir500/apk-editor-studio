#include "windows/permissioneditor.h"
#include "base/application.h"
#include <QScrollArea>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QDesktopServices>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

PermissionEditor::PermissionEditor(Manifest *manifest, QWidget *parent) : QDialog(parent), manifest(manifest)
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

    grid = new QGridLayout(viewport);
    auto permissions = manifest->getPermissionList();
    for (const auto &permission : permissions) {
        addPermissionLine(permission);
    }

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

void PermissionEditor::addPermissionLine(const Permission &permission)
{
    const int row = grid->rowCount();

    auto permissionName = permission.getName();
    auto labelTitle = new QLabel(permissionName, this);
    grid->addWidget(labelTitle, row, 0);

    auto btnHelp = new QToolButton(this);
    btnHelp->setToolTip(tr("Documentation"));
    btnHelp->setIcon(app->icons.get("help.png"));
    connect(btnHelp, &QToolButton::clicked, [=]() {
        const QString url("https://developer.android.com/reference/android/Manifest.permission.html#%1");
        QDesktopServices::openUrl(url.arg(permissionName.mid(QString("android.permission.").length())));
    });
    grid->addWidget(btnHelp, row, 1);
    if (!permissionName.startsWith("android.permission.")) {
        btnHelp->setEnabled(false);
    }

    auto btnRemove = new QToolButton(this);
    btnRemove->setToolTip(tr("Remove"));
    btnRemove->setIcon(app->icons.get("remove.png"));
    connect(btnRemove, &QToolButton::clicked, [=]() {
        delete labelTitle;
        delete btnHelp;
        delete btnRemove;
        manifest->removePermission(permission);
    });
    grid->addWidget(btnRemove, row, 2);
}
