#include "widgets/resourcesbaseview.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include <QFileDialog>

QSharedPointer<ResourceMenu> ResourcesBaseView::generateContextMenu(const QPersistentModelIndex &resourceIndex, const QString &resourcePath, QWidget *parent)
{
    if (resourcePath.isEmpty()) {
        return nullptr;
    }

    auto menu = new ResourceMenu(parent);
    menu->getSaveAction()->setVisible(false);

    menu->connect(menu, &ResourceMenu::editClicked, [=]() {
        emit editRequested(resourceIndex);
    });

    menu->connect(menu, &ResourceMenu::replaceClicked, [=]() {
        const bool isImage = app->formats.extensionsImages().contains(QFileInfo(resourcePath).suffix());
        if (isImage ? Dialogs::replaceImage(resourcePath, menu) : Dialogs::replaceFile(resourcePath, menu)) {
            auto model = const_cast<QAbstractItemModel *>(resourceIndex.model());
            emit model->dataChanged(resourceIndex, resourceIndex);
        }
    });

    menu->connect(menu, &ResourceMenu::saveAsClicked, [=]() {
        const QString src = resourcePath;
        const QString dst = QFileDialog::getSaveFileName(parent, QString(), src);
        if (!dst.isEmpty()) {
            QFile::copy(src, dst);
        }
    });

    menu->connect(menu, &ResourceMenu::exploreClicked, [=]() {
        app->explore(resourcePath);
    });

    return QSharedPointer<ResourceMenu>(menu);
}
