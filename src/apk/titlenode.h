#ifndef TITLENODE_H
#define TITLENODE_H

#include "apk/xmlnode.h"
#include "apk/resourcefile.h"

class TitleNode
{
public:
    TitleNode(XmlNode *node, ResourceFile *file);
    ~TitleNode();

    bool save() const;

    XmlNode *node;
    const ResourceFile *file;
};

#endif // TITLENODE_H
