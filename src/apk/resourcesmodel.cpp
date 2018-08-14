#include "apk/resourcesmodel.h"
#include "base/application.h"
#include "base/debug.h"
#include <QFileIconProvider>

ResourcesModel::ResourcesModel(QObject *parent) : QAbstractItemModel(parent)
{
    root = new ResourceNode();
}

ResourcesModel::~ResourcesModel()
{
    delete root;
}

QModelIndex ResourcesModel::addNode(ResourceNode *node, const QModelIndex &parent)
{
    ResourceNode *parentNode = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
        parentNode->addChild(node);
    endInsertRows();
    return createIndex(rowCount(parent) - 1, 0, node);
}

QVariant ResourcesModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        const ResourceNode *node = static_cast<ResourceNode *>(index.internalPointer());
        const int column = index.column();
        if (role == Qt::DisplayRole && column == NodeCaption) {
            return node->getCaption();
        }
        ResourceFile *file = node->getFile();
        if (file) {
            if (role == Qt::DisplayRole) {
                switch (column) {
                    case ResourceLanguage:   return file->getLanguageName();
                    case ResourceLocale:     return file->getLocaleCode();
                    case ResourceApi:        return file->getApiVersion();
                    case ResourceQualifiers: return file->getReadableQualifiers();
                    case ResourcePath:       return file->getFilePath();
                }
            } else if (role == Qt::DecorationRole) {
                switch (column) {
                    case NodeCaption: {
                        QFileInfo fileInfo(file->getFilePath());
                        const QString suffix = fileInfo.suffix();
                        if (app->formats.extensionsImages().contains(suffix, Qt::CaseInsensitive)) {
                            QPixmap thumbnail(file->getFilePath());
                            return thumbnail;
                        } else {
                            return QFileIconProvider().icon(fileInfo);
                        }
                        break;
                    }
                    case ResourceLanguage:
                        return file->getLanguageIcon();
                }
            }
        }
    }
    return QVariant();
}

QVariant ResourcesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case NodeCaption:        return tr("Resource");
            case ResourceLanguage:   return tr("Language");
            case ResourceLocale:     return tr("Locale");
            //: This string refers to the Android qualifiers (https://developer.android.com/guide/topics/resources/providing-resources).
            case ResourceQualifiers: return tr("Qualifiers");
            case ResourcePath:       return tr("Path");
            case ResourceApi:        return "API";
        }
    }
    return QVariant();
}

QModelIndex ResourcesModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex ResourcesModel::parent(const QModelIndex &index) const
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

int ResourcesModel::rowCount(const QModelIndex &parent) const
{
    ResourceNode *parentItem = parent.isValid() ? static_cast<ResourceNode *>(parent.internalPointer()) : root;
    return parentItem->childCount();
}

int ResourcesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QString ResourcesModel::getResourcePath(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    return index.sibling(index.row(), ResourcesModel::ResourcePath).data().toString();
}

QPixmap ResourcesModel::getResourceFlag(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    return index.sibling(index.row(), ResourcesModel::ResourceLanguage).data(Qt::DecorationRole).value<QPixmap>();
}
