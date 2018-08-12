#include "apk/xmlnode.h"
#include "base/debug.h"

XmlNode::XmlNode(const QDomElement &node, bool keepDocument)
{
    this->node = node;
    if (keepDocument) {
        document = node.ownerDocument();
    }
    modified = false;
}

XmlNode::~XmlNode()
{
    qDeleteAll(children);
}

QString XmlNode::getTagName() const
{
    return node.nodeName();
}

QString XmlNode::getAttribute(const QString &attribute) const
{
    return node.attribute(attribute);
}

QString XmlNode::getValue() const
{
    return node.firstChild().nodeValue();
}

QString XmlNode::getReadableType() const
{
    const QString tag = getTagName();
    if (tag == "string") {
        return tr("String");
    } else if (tag == "string-array") {
        return tr("String array");
    } else if (tag == "color") {
        return tr("Color");
    } else if (tag == "dimen") {
        return tr("Dimension");
    } else if (tag == "plurals") {
        return tr("Plurals");
    } else if (tag == "item") {
        return tr("Item");
    } else if (tag == "integer") {
        return tr("Integer");
    } else if (tag == "integer-array") {
        return tr("Integer Array");
    } else if (tag == "array") {
        return tr("Array");
    } else {
        return tag;
    }
}

QDomDocument XmlNode::getDocument() const
{
    return document;
}

bool XmlNode::wasModified() const
{
    return modified;
}

void XmlNode::setAttribute(const QString &attribute, const QString &value)
{
    node.setAttribute(attribute, value);
    modified = true;
}

void XmlNode::setValue(const QString &value)
{
    node.firstChild().setNodeValue(value);
    modified = true;
}

void XmlNode::addChild(XmlNode *child)
{
    child->parent = this;
    children.append(child);
}

XmlNode *XmlNode::getChild(int row)
{
    return children.at(row);
}

XmlNode *XmlNode::getParent()
{
    return parent;
}

int XmlNode::childCount() const
{
    return children.count();
}

int XmlNode::row() const
{
    return parent ? 0 : children.indexOf(const_cast<XmlNode *>(this));
}
