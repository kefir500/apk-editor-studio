#include "apk/resourcenode.h"
#include <QFile>

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
    delete this->file;
    this->file = file;
}

bool ResourceNode::removeFile(int row)
{
    auto child = getChild(row);
    return QFile::remove(child->file->getFilePath());
}

ResourceNode *ResourceNode::getChild(int row) const
{
    return static_cast<ResourceNode *>(children.at(row));
}

ResourceNode *ResourceNode::getParent() const
{
    return static_cast<ResourceNode *>(parent);
}
