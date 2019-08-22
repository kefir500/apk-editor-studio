#include "apk/resourceitemsmodel.h"
#include "base/utils.h"
#include <QIcon>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

ResourceItemsModel::ResourceItemsModel(const Project *apk, QObject *parent) : QAbstractItemModel(parent)
{
    this->apk = apk;
    root = new ResourceNode();
}

ResourceItemsModel::~ResourceItemsModel()
{
    delete root;
}

QModelIndex ResourceItemsModel::addNode(ResourceNode *node, const QModelIndex &parent)
{
    ResourceNode *parentNode = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
        parentNode->addChild(node);
    endInsertRows();
    auto index = createIndex(rowCount(parent) - 1, 0, node);
    emit added(index);
    return index;
}

bool ResourceItemsModel::replaceResource(const QModelIndex &index, const QString &with)
{
    const QString what = index.data(PathRole).toString();
    if (Utils::replaceFile(what, with)) {
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool ResourceItemsModel::removeResource(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    return removeRow(index.row(), index.parent());
}

QVariant ResourceItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const ResourceNode *node = static_cast<ResourceNode *>(index.internalPointer());
        const int column = index.column();
        switch (role) {
        case Qt::DisplayRole:
        case SortRole:
            if (column == NodeCaption) {
                return node->getCaption();
            }
        }
        ResourceFile *file = node->getFile();
        if (file) {
            switch (role) {
            case SortRole:
                switch (column) {
                case ResourceDpi: {
                    auto dpi = file->getDpi().toLower();
                    if (dpi == "ldpi") { return 0; }
                    if (dpi == "mdpi") { return 1; }
                    if (dpi == "hdpi") { return 2; }
                    if (dpi == "xhdpi") { return 3; }
                    if (dpi == "xxhdpi") { return 4; }
                    if (dpi == "xxxhdpi") { return 5; }
                    if (dpi == "nodpi") { return 6; }
                    if (dpi == "tvdpi") { return 7; }
                    if (dpi == "anydpi") { return 8; }
                    return 9;
                }
                case ResourceApi:
                    return file->getApiVersion().remove(0, 1).toInt();
                }
            case Qt::DisplayRole:
                switch (column) {
                case ResourceLanguage:
                    return file->getLanguageName();
                case ResourceLocale:
                    return file->getLocaleCode();
                case ResourceDpi:
                    return file->getDpi();
                case ResourceApi:
                    return file->getApiVersion();
                case ResourceQualifiers:
                    return file->getReadableQualifiers();
                case ResourcePath:
                    return file->getFilePath();
                }
                break;
            case PathRole:
                return file->getFilePath();
            case IconRole:
                return file->getFileIcon();
            case Qt::DecorationRole:
                switch (column) {
                case NodeCaption:
                    return file->getFileIcon();
                case ResourceLanguage:
                    return file->getLanguageIcon();
                }
                break;
            case ResourceNameRole:
                return file->getName();
            case ResourceTypeRole:
                return file->getType();
            }
        }
    }
    return QVariant();
}

QVariant ResourceItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case NodeCaption:        return tr("Resource");
            case ResourceLanguage:   return tr("Language");
            case ResourceLocale:     return tr("Locale");
            //: This string refers to the Android qualifiers (https://developer.android.com/guide/topics/resources/providing-resources).
            case ResourceQualifiers: return tr("Qualifiers");
            case ResourcePath:       return tr("Path");
            case ResourceDpi:        return "DPI";
            case ResourceApi:        return "API";
        }
    }
    return QVariant();
}

QModelIndex ResourceItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        ResourceNode *parentNode = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
        ResourceNode *childNode = parentNode->getChild(row);
        if (childNode) {
            return createIndex(row, column, childNode);
        }
    }
    return QModelIndex();
}

QModelIndex ResourceItemsModel::parent(const QModelIndex &index) const
{
    if (index.isValid()) {
        ResourceNode *childNode = static_cast<ResourceNode *>(index.internalPointer());
        ResourceNode *parentNode = childNode->getParent();
        if (parentNode != root) {
            return createIndex(parentNode->row(), 0, parentNode);
        }
    }
    return QModelIndex();
}

int ResourceItemsModel::rowCount(const QModelIndex &parent) const
{
    ResourceNode *parentItem = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
    return parentItem->childCount();
}

int ResourceItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

bool ResourceItemsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool success = true;
    auto node = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        if (!node->removeChild(row)) {
            success = false;
        }
    }
    endRemoveRows();
    return success;
}

QModelIndex ResourceItemsModel::findIndex(const QString &path) const
{
    return findIndex(path, {});
}

QModelIndex ResourceItemsModel::findIndex(const QString &path, const QModelIndex &parent) const
{
    for (int row = 0; row < rowCount(parent); ++row) {
        auto resource = index(row, ResourcePath, parent);
        if (resource.data().toString() == path) {
            return resource;
        }
        resource = findIndex(path, resource);
        if (resource.isValid()) {
            return resource;
        }
    }
    return {};
}

const ResourceFile *ResourceItemsModel::getResource(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return nullptr;
    }
    const ResourceNode *node = static_cast<ResourceNode *>(index.internalPointer());
    ResourceFile *resource = node->getFile();
    return resource;
}

const Project *ResourceItemsModel::getApk() const
{
    return apk;
}
