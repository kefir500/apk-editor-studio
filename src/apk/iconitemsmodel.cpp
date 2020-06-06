#include "apk/iconitemsmodel.h"
#include "base/application.h"
#include "base/utils.h"
#include <QDebug>

IconItemsModel::IconItemsModel(QObject *parent) : QAbstractProxyModel(parent)
{
    root = new TreeNode;
    applicationNode = new TreeNode;
    activitiesNode = new TreeNode;
    beginInsertRows({}, 0, 1);
    root->addChild(applicationNode);
    root->addChild(activitiesNode);
    endInsertRows();
}

IconItemsModel::~IconItemsModel()
{
    delete root;
}

void IconItemsModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    if (sourceModel()) {
        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &IconItemsModel::sourceRowsInserted);
        disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &IconItemsModel::sourceRowsAboutToBeRemoved);
        disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this, &IconItemsModel::sourceDataChanged);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &IconItemsModel::sourceModelReset);
    }
    Q_ASSERT(qobject_cast<ResourceItemsModel *>(newSourceModel));
    QAbstractProxyModel::setSourceModel(newSourceModel);
    if (sourceModel()) {
        sourceModelReset();
        connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &IconItemsModel::sourceRowsInserted);
        connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, &IconItemsModel::sourceRowsAboutToBeRemoved);
        connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &IconItemsModel::sourceDataChanged);
        connect(sourceModel(), &QAbstractItemModel::modelReset, this, &IconItemsModel::sourceModelReset);
    }
}

ResourceItemsModel *IconItemsModel::sourceModel() const
{
    return static_cast<ResourceItemsModel *>(QAbstractProxyModel::sourceModel());
}

QIcon IconItemsModel::getIcon() const
{
    QIcon icon;
    for (auto node : applicationNode->getChildren()) {
        auto iconNode = static_cast<IconNode *>(node);
        if (iconNode->type == TypeIcon) {
            const QPixmap pixmap = Utils::iconToPixmap(proxyToSourceMap.value(iconNode).data(Qt::DecorationRole).value<QIcon>());
            icon.addPixmap(pixmap);
        }
    }
    return icon;
}

QIcon IconItemsModel::getIcon(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (Q_LIKELY(sourceIndex.isValid())) {
        return sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::CaptionColumn).data(Qt::DecorationRole).value<QIcon>();
    }
    return QIcon();
}

QString IconItemsModel::getIconPath(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (Q_LIKELY(sourceIndex.isValid())) {
        return sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::PathColumn).data().toString();
    }
    return QString();
}

QString IconItemsModel::getIconCaption(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    auto iconNode = static_cast<IconNode *>(index.internalPointer());
    const QModelIndex sourceIndex = proxyToSourceMap.value(iconNode);
    QString caption = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::QualifiersColumn).data().toString().toUpper();
    switch (iconNode->type) {
    case TypeIcon:
        break;
    case TypeRoundIcon:
        caption.append(QString(" (%1)").arg(tr("Round icon")));
        break;
    case TypeBanner:
        caption.append(QString(" (%1)").arg(tr("TV banner")));
        break;
    }
    return caption;
}

IconItemsModel::IconType IconItemsModel::getIconType(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return TypeIcon;
    }
    auto node = static_cast<IconNode *>(index.internalPointer());
    return node->type;
}

bool IconItemsModel::replaceApplicationIcons(const QString &path, QWidget *parent)
{
    if (path.isEmpty()) {
        return false;
    }
    bool success = false;
    auto applicationIndex = index(ApplicationRow, 0);
    auto applicationIconCount = applicationNode->childCount();
    for (int row = 0; row < applicationIconCount; ++row) {
        auto iconIndex = index(row, PathColumn, applicationIndex);
        auto iconType = getIconType(iconIndex);
        if (iconType == TypeIcon || iconType == TypeRoundIcon) {
            if (Utils::isImageWritable(getIconPath(iconIndex))) {
                if (!sourceModel()->replaceResource(mapToSource(iconIndex), path, parent)) {
                    success = false;
                }
            }
        }
    }
    return success;
}

bool IconItemsModel::replaceResource(const QModelIndex &index, const QString &path, QWidget *parent)
{
    return sourceModel()->replaceResource(mapToSource(index), path, parent);
}

bool IconItemsModel::removeResource(const QModelIndex &index)
{
    return removeRow(index.row(), index.parent());
}

QString IconItemsModel::getResourcePath(const QModelIndex &index) const
{
    return getIconPath(index);
}

QVariant IconItemsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case CaptionColumn: {
                auto node = static_cast<TreeNode *>(index.internalPointer());
                if (node == applicationNode) {
                    return tr("Application");
                } else if (node == activitiesNode) {
                    //: This string refers to the Android activities (https://developer.android.com/guide/components/activities).
                    return tr("Activities");
                } else if (activitiesNode->hasChild(node)) {
                    return static_cast<ActivityNode *>(node)->scope->name();
                } else {
                    return getIconCaption(index);
                }
            }
            case PathColumn:
                return getIconPath(index);
            case TypeColumn:
                return getIconType(index);
            }
        } else if (role == Qt::ToolTipRole) {
            return getIconPath(index);
        } else if (role == Qt::DecorationRole) {
            if (index.column() == 0) {
                return getIcon(index);
            }
        }
    }
    return QVariant();
}

QModelIndex IconItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        auto parentNode = parent.isValid()
            ? static_cast<TreeNode *>(parent.internalPointer())
            : root;
        return createIndex(row, column, parentNode->getChild(row));
    }
    return QModelIndex();
}

QModelIndex IconItemsModel::parent(const QModelIndex &index) const
{
    if (index.isValid()) {
        auto childNode = static_cast<TreeNode *>(index.internalPointer());
        if (childNode) {
            auto parentNode = childNode->getParent();
            if (parentNode != root) {
                return createIndex(parentNode->row(), 0, parentNode);
            }
        }
    }
    return QModelIndex();
}

QModelIndex IconItemsModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (sourceIndex.isValid()) {
        auto iconNode = sourceToProxyMap.value(sourceIndex);
        if (iconNode) {
            return createIndex(iconNode->row(), sourceIndex.column(), iconNode);
        }
    }
    return QModelIndex();
}

QModelIndex IconItemsModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (proxyIndex.isValid()) {
        auto proxyNode = static_cast<TreeNode *>(proxyIndex.internalPointer());
        if (!proxyNode->hasChildren()) {
            auto iconNode = static_cast<IconNode *>(proxyNode);
            return proxyToSourceMap.value(iconNode);
        }
    }
    return QModelIndex();
}

void IconItemsModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    const auto applicationIndex = index(ApplicationRow, 0);
    const auto activitiesIndex = index(ActivitiesRow, 0);
    const QList<QPersistentModelIndex> rootIndexes{applicationIndex, activitiesIndex};
    emit layoutAboutToBeChanged(rootIndexes, QAbstractItemModel::VerticalSortHint);

    auto comparator = [this](TreeNode *node1, TreeNode *node2) -> bool {
        auto icon1 = static_cast<IconNode *>(node1);
        auto icon2 = static_cast<IconNode *>(node2);
        auto index1 = proxyToSourceMap.value(icon1);
        auto index2 = proxyToSourceMap.value(icon2);
        const bool swap = ![&]() -> bool {
            if (icon1->type < icon2->type) { return true; }
            if (icon2->type < icon1->type) { return false; }
            auto dpi1 = index1.sibling(index1.row(), ResourceItemsModel::DpiColumn).data(ResourceItemsModel::SortRole);
            auto dpi2 = index2.sibling(index2.row(), ResourceItemsModel::DpiColumn).data(ResourceItemsModel::SortRole);
            if (dpi1 < dpi2) { return true; }
            if (dpi2 < dpi1) { return false; }
            return false;
        }();
        if (swap) {
            changePersistentIndex(mapFromSource(index1), mapFromSource(index2));
        }
        return !swap;
    };

    auto &applicationIcons = applicationNode->getChildren();
    std::sort(applicationIcons.begin(), applicationIcons.end(), comparator);

    auto &activityScopes = activitiesNode->getChildren();
    std::sort(activityScopes.begin(), activityScopes.end(), [](const TreeNode *node1, const TreeNode *node2) -> bool {
        auto activity1 = static_cast<const ActivityNode *>(node1);
        auto activity2 = static_cast<const ActivityNode *>(node2);
        return activity1->scope->type() < activity2->scope->type();
    });

    for (auto activityNode : activityScopes) {
        auto &activityIcons = activityNode->getChildren();
        std::sort(activityIcons.begin(), activityIcons.end(), comparator);
    }

    emit layoutChanged(rootIndexes, QAbstractItemModel::VerticalSortHint);
}

int IconItemsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return static_cast<TreeNode *>(parent.internalPointer())->childCount();
    } else {
        return applicationNode->hasChildren() + activitiesNode->hasChildren();
    }
}

int IconItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

bool IconItemsModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return static_cast<TreeNode *>(parent.internalPointer())->hasChildren();
    } else {
        return applicationNode->hasChildren() || activitiesNode->hasChildren();
    }
}

bool IconItemsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    auto index = mapToSource(this->index(row, 0, parent));
    if (!index.isValid()) {
        return false;
    }
    return sourceModel()->removeRows(index.row(), count, index.parent());
}

bool IconItemsModel::appendIcon(const QPersistentModelIndex &index, ManifestScope *scope, IconType type)
{
    if (!sourceToProxyMap.contains(index)) {
        switch (scope->type()) {
        case ManifestScope::Type::Application: {
            const int row = applicationNode->childCount();
            beginInsertRows(this->index(ApplicationRow, 0), row, row);
                auto iconNode = new IconNode(type);
                applicationNode->addChild(iconNode);
                sourceToProxyMap.insert(index, iconNode);
                proxyToSourceMap.insert(iconNode, index);
            endInsertRows();
            sort();
            return true;
        }
        case ManifestScope::Type::Activity: {
            ActivityNode *activityNode = nullptr;
            // Try to find existing activity node:
            for (auto node : activitiesNode->getChildren()) {
                auto interimActivityNode = static_cast<ActivityNode *>(node);
                if (scope == interimActivityNode->scope) {
                    activityNode = interimActivityNode;
                    break;
                }
            }
            if (!activityNode) {
                // Create new activity node:
                const int row = activitiesNode->childCount();
                beginInsertRows(this->index(ActivitiesRow, 0), row, row);
                    activityNode = new ActivityNode(scope);
                    activitiesNode->addChild(activityNode);
                endInsertRows();
            }
            const int row = activityNode->childCount();
            beginInsertRows(this->index(activityNode->row(), 0, this->index(ActivitiesRow, 0)), row, row);
                auto iconNode = new IconNode(type);
                activityNode->addChild(iconNode);
                sourceToProxyMap.insert(index, iconNode);
                proxyToSourceMap.insert(iconNode, index);
            endInsertRows();
            sort();
            return true;
        }
        }
    }
    return false;
}

void IconItemsModel::populateFromSource(const QModelIndex &parent)
{
    const int rows = sourceModel()->rowCount(parent);
    sourceRowsInserted(parent, 0, rows);
    for (int row = 0; row < rows; ++row) {
        populateFromSource(sourceModel()->index(row, 0, parent));
    }
}

void IconItemsModel::sourceRowsInserted(const QModelIndex &parent, int first, int last)
{
    for (int row = first; row <= last; ++row) {
        const auto index = sourceModel()->index(row, 0, parent);
        const auto resource = sourceModel()->getResourceFile(index);
        if (resource && Utils::isDrawableResource(resource->getFilePath())) {
            auto resourceName = resource->getName();
            auto resourceType = resource->getType();
            if (!resourceName.isEmpty() && !resourceType.isEmpty()) {
                for (ManifestScope *scope : apk()->manifest->scopes) {
                    if (resourceName == scope->icon().getResourceName() && resourceType == scope->icon().getResourceType()) {
                        appendIcon(index, scope, TypeIcon);
                    } else if (resourceName == scope->roundIcon().getResourceName() && resourceType == scope->roundIcon().getResourceType()) {
                        appendIcon(index, scope, TypeRoundIcon);
                    } else if (resourceName == scope->banner().getResourceName() && resourceType == scope->banner().getResourceType()) {
                        appendIcon(index, scope, TypeBanner);
                    }
                }
            }
        }
    }
}

void IconItemsModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    for (int row = first; row <= last; ++row) {
        auto sourceIndex = sourceModel()->index(row, 0, parent);
        auto proxyIndex = mapFromSource(sourceIndex);
        if (proxyIndex.isValid()) {
            auto proxyRow = proxyIndex.row();
            beginRemoveRows(proxyIndex.parent(), proxyRow, proxyRow);
                auto proxyNode = sourceToProxyMap.value(sourceIndex);
                proxyNode->removeSelf();
                proxyToSourceMap.remove(proxyNode);
                sourceToProxyMap.remove(sourceIndex);
            endRemoveRows();
        }
    }
}

void IconItemsModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

void IconItemsModel::sourceModelReset()
{
    beginResetModel();
    sourceToProxyMap.clear();
    proxyToSourceMap.clear();
    applicationNode->removeChildren();
    activitiesNode->removeChildren();
    populateFromSource();
    endResetModel();
}

const Project *IconItemsModel::apk() const
{
    return sourceModel()->getApk();
}

void IconItemsModel::ActivityNode::addChild(IconItemsModel::IconNode *node)
{
    TreeNode::addChild(node);
}
