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
    virtual bool removeChild(int row);
    void removeChildren();
    bool removeSelf();
    int childCount() const;
    QVector<TreeNode *> &getChildren();
    TreeNode *getChild(int row) const;
    TreeNode *findChild(TreeNode *node) const;
    TreeNode *getParent() const;
    int row() const;

protected:
    TreeNode *parent;
    QVector<TreeNode *> children;
};


#endif // TREENODE_H
