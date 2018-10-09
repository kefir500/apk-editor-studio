#ifndef RESOURCENODE_H
#define RESOURCENODE_H

#include "apk/resourcefile.h"

class ResourceNode
{
public:
    ResourceNode(const QString &caption = QString(), ResourceFile *file = nullptr);
    ~ResourceNode();

    QString getCaption() const;
    ResourceFile *getFile() const;

    void setCaption(const QString &caption);
    void setFile(ResourceFile *file);

    void addChild(ResourceNode *child);
    int childCount() const;
    ResourceNode *getChild(int row);
    ResourceNode *getParent() const;
    int row() const;

private:
    QString caption;
    ResourceFile *file;
    ResourceNode *parent;
    QList<ResourceNode *> children;
};

#endif // RESOURCENODE_H
