#ifndef RESOURCENODE_H
#define RESOURCENODE_H

#include "base/treenode.h"
#include "apk/resourcefile.h"

class ResourceNode : public TreeNode
{
public:
    ResourceNode(const QString &caption = QString(), ResourceFile *file = nullptr);
    ~ResourceNode();

    QString getCaption() const;
    ResourceFile *getFile() const;

    void setCaption(const QString &caption);
    void setFile(ResourceFile *file);

    void addChild(ResourceNode *child);
    ResourceNode *getChild(int row) const;
    ResourceNode *getParent() const;

private:
    QString caption;
    ResourceFile *file;
};

#endif // RESOURCENODE_H
