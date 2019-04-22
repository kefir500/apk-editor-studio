#include "apk/resourceitemsmodel.h"
#include "base/utils.h"
#include <QFileIconProvider>

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

QVariant ResourceItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const ResourceNode *node = static_cast<ResourceNode *>(index.internalPointer());
        const int column = index.column();
        if (role == Qt::DisplayRole && column == NodeCaption) {
            return node->getCaption();
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
                    case ResourceLanguage:   return file->getLanguageName();
                    case ResourceLocale:     return file->getLocaleCode();
                    case ResourceDpi:        return file->getDpi();
                    case ResourceApi:        return file->getApiVersion();
                    case ResourceQualifiers: return file->getReadableQualifiers();
                    case ResourcePath:       return file->getFilePath();
                }
            case Qt::DecorationRole:
                switch (column) {
                    case NodeCaption: {
                        const QString filePath = file->getFilePath();
                        if (Utils::isImageReadable(filePath)) {
                            QPixmap thumbnail(filePath);
                            return thumbnail;
                        }
                        return QFileIconProvider().icon(QFileInfo(filePath));
                    }
                    case ResourceLanguage:
                        return file->getLanguageIcon();
                }
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
        ResourceNode *parentItem = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
        ResourceNode *childItem = parentItem->getChild(row);
        if (childItem) {
            return createIndex(row, column, childItem);
        }
    }
    return QModelIndex();
}

QModelIndex ResourceItemsModel::parent(const QModelIndex &index) const
{
    if (index.isValid()) {
        ResourceNode *childItem = static_cast<ResourceNode *>(index.internalPointer());
        ResourceNode *parentItem = childItem->getParent();
        if (parentItem != root) {
            return createIndex(parentItem->row(), 0, parentItem);
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
