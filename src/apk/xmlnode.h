#ifndef XMLNODE_H
#define XMLNODE_H

#include <QList>
#include <QDomElement>
#include <QCoreApplication>

class XmlNode
{
    Q_DECLARE_TR_FUNCTIONS(XmlNode)

public:
    explicit XmlNode(const QDomElement &node, bool keepDocument = false);
    ~XmlNode();

    QString getTagName() const;
    QString getAttribute(const QString &attribute) const;
    QString getValue() const;
    QString getReadableType() const;
    QDomDocument getDocument() const;

    bool wasModified() const;

    void setAttribute(const QString &attribute, const QString &value);
    void setValue(const QString &value);

    void addChild(XmlNode *getChild);
    XmlNode *getChild(int row);
    XmlNode *getParent();
    int childCount() const;
    int row() const;

private:
    QDomElement node;
    QDomDocument document;

    bool modified;

    QList<XmlNode *> children;
    XmlNode *parent;
};

#endif // XMLNODE_H
