#include "apk/iconitemsmodel.h"
#include "base/application.h"
#include "base/utils.h"
#include <QFileIconProvider>
#include <QDebug>

IconItemsModel::IconItemsModel(QObject *parent) : QAbstractProxyModel(parent)
{
    applicationNode = new TreeNode();
    activitiesNode = new TreeNode();
}

IconItemsModel::~IconItemsModel()
{
    delete applicationNode;
    delete activitiesNode;
}

void IconItemsModel::setSourceModel(ResourceItemsModel *newSourceModel)
{
    beginResetModel();
        if (sourceModel()) {
            disconnect(sourceModel(), &ResourceItemsModel::added, this, &IconItemsModel::onResourceAdded);
            disconnect(sourceModel(), &ResourceItemsModel::removed, this, &IconItemsModel::onResourceRemoved);
            disconnect(sourceModel(), &ResourceItemsModel::dataChanged, this, &IconItemsModel::onResourceChanged);
            disconnect(apk(), &Project::unpacked, this, &IconItemsModel::ready);
        }
        QAbstractProxyModel::setSourceModel(newSourceModel);
        if (sourceModel()) {
            connect(sourceModel(), &ResourceItemsModel::added, this, &IconItemsModel::onResourceAdded);
            connect(sourceModel(), &ResourceItemsModel::removed, this, &IconItemsModel::onResourceRemoved);
            connect(sourceModel(), &ResourceItemsModel::dataChanged, this, &IconItemsModel::onResourceChanged);
            connect(apk(), &Project::unpacked, this, &IconItemsModel::ready);
        }
    endResetModel();
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
        if (iconNode->type == Icon) {
            const QPixmap pixmap = proxyToSourceMap.value(iconNode).data(Qt::DecorationRole).value<QPixmap>();
            icon.addPixmap(pixmap);
        }
    }
    return icon;
}

QIcon IconItemsModel::getIcon(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (Q_LIKELY(sourceIndex.isValid())) {
        QIcon icon = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::NodeCaption).data(Qt::DecorationRole).value<QPixmap>();
        return !icon.isNull() ? icon : QFileIconProvider().icon(getIconPath(index));
    }
    return QIcon();
}

QString IconItemsModel::getIconPath(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (Q_LIKELY(sourceIndex.isValid())) {
        return sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourcePath).data().toString();
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
    QString caption = sourceIndex.sibling(sourceIndex.row(), ResourceItemsModel::ResourceQualifiers).data().toString().toUpper();
    switch (iconNode->type) {
    case Icon:
        break;
    case RoundIcon:
        caption.append(QString(" (%1)").arg(tr("Round icon")));
        break;
    case Banner:
        caption.append(QString(" (%1)").arg(tr("TV Banner")));
        break;
    }
    return caption;
}

IconItemsModel::IconType IconItemsModel::getIconType(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Icon;
    }
    auto node = static_cast<IconNode *>(index.internalPointer());
    return node->type;
}

void IconItemsModel::replaceIcon(const QModelIndex &index, const QString &iconSource)
{
    sourceModel()->replaceResource(mapToSource(index), iconSource);
}

void IconItemsModel::replaceApplicationIcons(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }
    auto applicationIndex = index(ApplicationRow, 0);
    auto applicationIconCount = applicationNode->childCount();
    for (int row = 0; row < applicationIconCount; ++row) {
        auto iconIndex = index(row, PathColumn, applicationIndex);
        auto iconType = getIconType(iconIndex);
        if (iconType == Icon || iconType == RoundIcon) {
            sourceModel()->replaceResource(mapToSource(iconIndex), path);
        }
    }
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
        if (!parent.isValid()) {
            switch (row) {
            case ApplicationRow:
                return createIndex(row, column, applicationNode);
            case ActivitiesRow:
                return createIndex(row, column, activitiesNode);
            }
        } else {
            auto parentNode = static_cast<TreeNode *>(parent.internalPointer());
            return createIndex(row, column, parentNode->getChild(row));
        }
    }
    return QModelIndex();
}

QModelIndex IconItemsModel::parent(const QModelIndex &index) const
{
    if (index.isValid()) {
        auto childNode = static_cast<TreeNode *>(index.internalPointer());
        if (childNode) {
            auto parentNode = childNode->getParent();
            if (parentNode) {
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
    if (proxyIndex.isValid() && proxyIndex.parent().isValid()) {
        auto node = static_cast<TreeNode *>(proxyIndex.internalPointer());
        auto iconNode = static_cast<IconNode *>(node);
        return proxyToSourceMap.value(iconNode);
    }
    return QModelIndex();
}

void IconItemsModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    auto sortIcons = [this](TreeNode *node1, TreeNode *node2) -> bool {
        auto icon1 = static_cast<IconNode *>(node1);
        auto icon2 = static_cast<IconNode *>(node2);
        auto index1 = proxyToSourceMap.value(icon1);
        auto index2 = proxyToSourceMap.value(icon2);
        if (icon1->type < icon2->type) { return true; }
        if (icon2->type < icon1->type) { return false; }
        auto dpi1 = index1.sibling(index1.row(), ResourceItemsModel::ResourceDpi).data(ResourceItemsModel::SortRole);
        auto dpi2 = index2.sibling(index2.row(), ResourceItemsModel::ResourceDpi).data(ResourceItemsModel::SortRole);
        if (dpi1 < dpi2) { return true; }
        if (dpi2 < dpi1) { return false; }
        return false;
    };

    beginResetModel();

    auto &applicationIcons = applicationNode->getChildren();
    std::sort(applicationIcons.begin(), applicationIcons.end(), sortIcons);

    auto &activityScopes = activitiesNode->getChildren();
    std::sort(activityScopes.begin(), activityScopes.end(), [](const TreeNode *node1, const TreeNode *node2) -> bool {
        auto activity1 = static_cast<const ActivityNode *>(node1);
        auto activity2 = static_cast<const ActivityNode *>(node2);
        return activity1->scope->type() < activity2->scope->type();
    });

    for (auto activityNode : activityScopes) {
        auto &activityIcons = activityNode->getChildren();
        std::sort(activityIcons.begin(), activityIcons.end(), sortIcons);
    }

    endResetModel();
}

int IconItemsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid()
        ? static_cast<TreeNode *>(parent.internalPointer())->childCount()
        : RowCount;
}

int IconItemsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
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
            return true;
        }
        }
    }
    return false;
}

void IconItemsModel::onResourceAdded(const QModelIndex &index)
{
    auto resource = sourceModel()->getResource(index);
    if (resource && Utils::isDrawableResource(resource->getFilePath())) {
        auto resourceName = resource->getName();
        auto resourceType = resource->getType();
        if (!resourceName.isEmpty() && !resourceType.isEmpty()) {
            for (ManifestScope *scope : apk()->getManifest()->scopes) {
                if (resourceName == scope->icon().getResourceName() && resourceType == scope->icon().getResourceType()) {
                    appendIcon(index, scope, Icon);
                } else if (resourceName == scope->roundIcon().getResourceName() && resourceType == scope->roundIcon().getResourceType()) {
                    appendIcon(index, scope, RoundIcon);
                } else if (resourceName == scope->banner().getResourceName() && resourceType == scope->banner().getResourceType()) {
                    appendIcon(index, scope, Banner);
                }
            }
        }
    }
}

void IconItemsModel::onResourceRemoved(const QModelIndex &index)
{
    // TODO
}

void IconItemsModel::onResourceChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

const Project *IconItemsModel::apk() const
{
    return sourceModel()->getApk();
}

void IconItemsModel::ActivityNode::addChild(IconItemsModel::IconNode *node)
{
    TreeNode::addChild(node);
}
