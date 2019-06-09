#include "apk/filesystemmodel.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

bool FileSystemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count)
    auto path = filePath(index(row, 0, parent));
    resourceModel->removeResource(path);
    return false;
}
