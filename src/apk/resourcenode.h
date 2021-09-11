#ifndef RESOURCENODE_H
#define RESOURCENODE_H

#include "base/treenode.h"
#include "apk/resourcefile.h"

class ResourceNode : public TreeNode
{
public:
    ResourceNode(const QString &caption = QString(), ResourceFile *file = nullptr);
    ~ResourceNode() override;

    QString getCaption() const;
    ResourceFile *getFile() const;

    void setCaption(const QString &caption);
    void setFile(ResourceFile *file);

    bool removeChild(int row) override;
    ResourceNode *getChild(int row) const;
    ResourceNode *getParent() const;

private:
    QString caption;
    ResourceFile *file;
};

#endif // RESOURCENODE_H
