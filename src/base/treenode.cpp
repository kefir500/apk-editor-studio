#include "base/treenode.h"

TreeNode::~TreeNode()
{
    removeChildren();
}

void TreeNode::addChild(TreeNode *node)
{
    node->parent = this;
    children.append(node);
}

bool TreeNode::hasChild(TreeNode *node) const
{
    return children.contains(node);
}

bool TreeNode::hasChildren() const
{
    return !children.isEmpty();
}

bool TreeNode::removeChild(int row)
{
    delete children[row];
    children.remove(row);
    return true;
}

void TreeNode::removeChildren()
{
    qDeleteAll(children);
    children.clear();
}

bool TreeNode::removeSelf()
{
    Q_ASSERT(parent);
    return parent->removeChild(row());
}

int TreeNode::childCount() const
{
    return children.count();
}

TreeNode *TreeNode::getChild(int row) const
{
    return children.at(row);
}

TreeNode *TreeNode::getParent() const
{
    return parent;
}

QVector<TreeNode *> &TreeNode::getChildren()
{
    return children;
}

int TreeNode::row() const
{
    return parent ? parent->children.indexOf(const_cast<TreeNode *>(this)) : 0;
}
