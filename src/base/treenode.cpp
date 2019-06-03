#include "base/treenode.h"

TreeNode::~TreeNode()
{
    qDeleteAll(children);
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

int TreeNode::childCount() const
{
    return children.count();
}

QVector<TreeNode *> &TreeNode::getChildren()
{
    return children;
}

TreeNode *TreeNode::getChild(int row) const
{
    return children.at(row);
}

TreeNode *TreeNode::findChild(TreeNode *node) const
{
    TreeNode *result = nullptr;
    auto it = std::find(children.begin(), children.end(), node);
    if (it != children.end()) {
        result = *it;
    } else {
        for (auto child : children) {
            result = child->findChild(node);
            if (result) {
                break;
            }
        }
    }
    return result;
}

TreeNode *TreeNode::getParent() const
{
    return parent;
}

int TreeNode::row() const
{
    return parent ? parent->children.indexOf(const_cast<TreeNode *>(this)) : 0;
}
