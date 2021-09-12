#ifndef TREENODE_H
#define TREENODE_H

#include <QVector>

class TreeNode
{
public:
    TreeNode() : parent(nullptr) {}
    virtual ~TreeNode();

    void addChild(TreeNode *node);
    bool hasChild(TreeNode *node) const;
    bool hasChildren() const;

    virtual void removeChild(int row);
    void removeChildren();
    void removeSelf();

    int childCount() const;
    TreeNode *getChild(int row) const;
    TreeNode *getParent() const;
    QVector<TreeNode *> &getChildren();

    int row() const;

protected:
    TreeNode *parent;
    QVector<TreeNode *> children;
};


#endif // TREENODE_H
