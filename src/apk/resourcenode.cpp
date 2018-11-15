#include "apk/resourcenode.h"

ResourceNode::ResourceNode(const QString &caption, ResourceFile *file)
{
    this->file = nullptr;
    this->parent = nullptr;
    setCaption(caption);
    setFile(file);
}

ResourceNode::~ResourceNode()
{
    delete file;
    qDeleteAll(children);
}

QString ResourceNode::getCaption() const
{
    return caption;
}

ResourceFile *ResourceNode::getFile() const
{
    return file;
}

void ResourceNode::setCaption(const QString &caption)
{
    this->caption = caption;
}

void ResourceNode::setFile(ResourceFile *file)
{
    if (this->file) {
        delete this->file;
    }
    this->file = file;
}

void ResourceNode::addChild(ResourceNode *child)
{
    children.append(child);
    child->parent = this;
}

ResourceNode *ResourceNode::getChild(int row)
{
    return children.at(row);
}

int ResourceNode::childCount() const
{
    return children.count();
}

ResourceNode *ResourceNode::getParent() const
{
    return parent;
}

int ResourceNode::row() const
{
    return parent ? parent->children.indexOf(const_cast<ResourceNode *>(this)) : 0;
}
