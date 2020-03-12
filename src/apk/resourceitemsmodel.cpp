#include "apk/resourceitemsmodel.h"
#include "apk/resourcemodelindex.h"
#include "base/utils.h"
#include <QtConcurrent/QtConcurrent>
#include <QDirIterator>
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

QFuture<void> ResourceItemsModel::initialize(const QString &path)
{
    beginResetModel();

    return QtConcurrent::run([=] {

        // Parse resource directories:

        QMap<QString, ResourceNode *> mapResourceTypes;
        QMap<QString, ResourceNode *> mapResourceGroups;

        QDirIterator resourceDirectories(path, QDir::Dirs | QDir::NoDotAndDotDot);
        while (resourceDirectories.hasNext()) {

            const QFileInfo resourceDirectory = QFileInfo(resourceDirectories.next());
            const QString resourceTypeTitle = resourceDirectory.fileName().split('-').first(); // E.g., "drawable", "values"...
            ResourceNode *resourceTypeNode = mapResourceTypes.value(resourceTypeTitle, nullptr);
            if (!resourceTypeNode) {
                resourceTypeNode = new ResourceNode(resourceTypeTitle, nullptr);
                root->addChild(resourceTypeNode);
                mapResourceTypes[resourceTypeTitle] = resourceTypeNode;
            }

            // Parse resource files:

            QDirIterator resourceFiles(resourceDirectory.filePath(), QDir::Files);
            while (resourceFiles.hasNext()) {

                const QFileInfo resourceFile(resourceFiles.next());
                const QString resourceFilename = resourceFile.fileName();

                ResourceNode *resourceGroupNode  = mapResourceGroups.value(resourceFilename, nullptr);
                if (!resourceGroupNode) {
                    resourceGroupNode = new ResourceNode(resourceFilename, nullptr);
                    resourceTypeNode->addChild(resourceGroupNode);
                    mapResourceGroups[resourceFilename] = resourceGroupNode;
                }

                ResourceNode *fileNode = new ResourceNode(resourceFilename, new ResourceFile(resourceFile.filePath()));
                resourceGroupNode->addChild(fileNode);
            }
        }

        endResetModel();
    });
}

QModelIndex ResourceItemsModel::addNode(ResourceNode *node, const QModelIndex &parent)
{
    ResourceNode *parentNode = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
        parentNode->addChild(node);
    endInsertRows();
    auto index = createIndex(rowCount(parent) - 1, 0, node);
    return index;
}

bool ResourceItemsModel::replaceResource(const QModelIndex &index, const QString &with)
{
    const QString what = ResourceModelIndex(index).path();
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

QString ResourceItemsModel::getResourcePath(const QModelIndex &index) const
{
    return index.sibling(index.row(), PathColumn).data().toString();
}

QVariant ResourceItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const ResourceNode *node = static_cast<ResourceNode *>(index.internalPointer());
        const int column = index.column();
        switch (role) {
        case Qt::DisplayRole:
        case SortRole:
            if (column == CaptionColumn) {
                return node->getCaption();
            }
        }
        ResourceFile *file = node->getFile();
        if (file) {
            switch (role) {
            case SortRole:
                switch (column) {
                case DpiColumn: {
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
                case ApiColumn:
                    return file->getApiVersion().remove(0, 1).toInt();
                }
            case Qt::DisplayRole:
                switch (column) {
                case LanguageColumn:
                    return file->getLanguageName();
                case LocaleColumn:
                    return file->getLocaleCode();
                case DpiColumn:
                    return file->getDpi();
                case ApiColumn:
                    return file->getApiVersion();
                case QualifiersColumn:
                    return file->getReadableQualifiers();
                case PathColumn:
                    return file->getFilePath();
                }
                break;
            case Qt::DecorationRole:
                switch (column) {
                case CaptionColumn:
                    return file->getFileIcon();
                case LanguageColumn:
                    return file->getLanguageIcon();
                }
                break;
            }
        }
    }
    return QVariant();
}

QVariant ResourceItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case CaptionColumn:
            return tr("Resource");
        case LanguageColumn:
            return tr("Language");
        case LocaleColumn:
            return tr("Locale");
        case QualifiersColumn:
            //: This string refers to the Android qualifiers (https://developer.android.com/guide/topics/resources/providing-resources).
            return tr("Qualifiers");
        case PathColumn:
            return tr("Path");
        case DpiColumn:
            return "DPI";
        case ApiColumn:
            return "API";
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
        auto resource = index(row, PathColumn, parent);
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

const ResourceFile *ResourceItemsModel::getResourceFile(const QModelIndex &index) const
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
