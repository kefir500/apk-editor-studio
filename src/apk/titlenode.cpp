#include "apk/titlenode.h"
#include <QFile>
#include <QDebug>

TitleNode::TitleNode(XmlNode *node, ResourceFile *file)
{
    this->node = node;
    this->file = file;
}

TitleNode::~TitleNode()
{
    delete node;
    delete file;
}

bool TitleNode::save() const
{
    if (node->wasModified()) {
        QFile xml(file->getFilePath());
        if (!xml.open(QFile::ReadWrite | QFile::Text)) {
            qWarning() << "Error: Could not save titles resource file";
            return false;
        }
        xml.resize(0);
        QTextStream stream(&xml);
        stream.setCodec("UTF-8");
        node->getDocument().save(stream, 4);
    }
    return true;
}
